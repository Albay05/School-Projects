#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "defs.h"

volatile sig_atomic_t stop_flag = 0;
char* USAGE_MESSAGE  =  "./ServerTrd -p <tcp_port> -u <udp_port> -s <stocks.txt> -l <logfile>\n"
                        "\nConstraints:"
                        "\n -p TCP port for trader and analyst connections (>=1024)"
                        "\n -u UDP port for ticker broadcast (>=1024)"
                        "\n -s Path to stock list file"
                        "\n -s Path to server log output file";

void print_usage() {
    fprintf(stderr, "%s\n", USAGE_MESSAGE);
    exit(EXIT_FAILURE);
}

void handle_sigint(int sig) {
    (void)sig;
    stop_flag = 1;
}

int main(int argc, char *argv[]) {
    if(argc <= 1) print_usage();

    int opt;
    int tcp_port = 0;
    int udp_port = 0;
    char* stock_file = NULL;
    char* logfile = NULL;

    while((opt = getopt(argc, argv, "p:u:s:l:")) != -1) {
        switch(opt) {
            case 'p':
                tcp_port = atoi(optarg);
                break;
            case 'u':
                udp_port = atoi(optarg);
                break;
            case 's':
                stock_file = optarg;
                break;
            case 'l':
                logfile = optarg;
                break;
            default:
                print_usage();
        }
    }

    if(tcp_port < 1024 || udp_port < 1024 || stock_file == NULL || logfile == NULL) {
        fprintf(stderr, "Missing or invalid parameter. Please obey the correct usage.\n");
        print_usage();
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if(sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    int stocks_read = read_stock_file(stock_file, NULL, 0);
    stock_t stocks_array[stocks_read];
    read_stock_file(stock_file, stocks_array, 1);

    // TCP setup
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) exit(EXIT_FAILURE);

    int opt_val = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(tcp_port);

    if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) exit(EXIT_FAILURE);
    if(listen(sockfd, 10) < 0) exit(EXIT_FAILURE);

    // UDP setup
    int u_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(u_sockfd < 0) exit(EXIT_FAILURE);

    setsockopt(u_sockfd, SOL_SOCKET, SO_BROADCAST, &opt_val, sizeof(opt_val));

    struct sockaddr_in u_server_addr;
    memset(&u_server_addr, 0, sizeof(u_server_addr));
    u_server_addr.sin_family = AF_INET;
    u_server_addr.sin_addr.s_addr = INADDR_ANY;
    u_server_addr.sin_port = htons(udp_port);

    client_t clients[MAX_CLIENTS];
    for(int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].fd = -1;
        memset(clients[i].username, 0, sizeof(clients[i].username));
        memset(clients[i].type, 0, sizeof(clients[i].type));
        memset(clients[i].line_buf, 0, sizeof(clients[i].line_buf));
        clients[i].portfolio_size = 0;
    }

    FILE* log_fptr = fopen(logfile, "w");

    struct timespec startup_time;
    clock_gettime(CLOCK_MONOTONIC, &startup_time);

    fprintf(log_fptr, "[SERVER] SERVER_START tcp_port=%d udp_port=%d stocks=%d\n", tcp_port, udp_port, stocks_read);
    printf("[SERVER] SERVER_START tcp_port=%d udp_port=%d stocks=%d\n", tcp_port, udp_port, stocks_read);
    fflush(log_fptr);

    server_state_t global_state;
    global_state.clients = clients; 
    global_state.stocks = stocks_array;
    global_state.num_stocks = stocks_read;
    global_state.u_sockfd = u_sockfd;
    global_state.log_fptr = log_fptr;

    memset(&global_state.bcast_addr, 0, sizeof(global_state.bcast_addr));
    global_state.bcast_addr.sin_family = AF_INET;
    global_state.bcast_addr.sin_port = htons(udp_port);
    global_state.bcast_addr.sin_addr.s_addr = inet_addr("255.255.255.255");

    while(!stop_flag) {
        struct timespec current_time;
        clock_gettime(CLOCK_MONOTONIC, &current_time);

        double elapsed = (current_time.tv_sec - startup_time.tv_sec) + 
                         (current_time.tv_nsec - startup_time.tv_nsec) / 1e9;

        struct timeval timeout;
        if(elapsed >= 5.0) {
            timeout.tv_sec = 0;
            timeout.tv_usec = 0;
        }
        else {
            double remaining = 5.0 - elapsed;
            timeout.tv_sec = (int)remaining; // in seconds
            timeout.tv_usec = (int)((remaining - timeout.tv_sec) * 1e6); // in microseconds
        }

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);

        int max_fd = sockfd;

        for(int i = 0; i < MAX_CLIENTS; i++) {
            int client_fd = clients[i].fd;

            if(client_fd > 0) {
                FD_SET(client_fd, &read_fds);

                if(client_fd > max_fd) max_fd = client_fd;
            }
        }

        int activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);
        
        if(activity < 0) break;

        if(activity == 0) {
            broadcast_all_prices(&global_state, "PERIODIC");

            clock_gettime(CLOCK_MONOTONIC, &startup_time);
        }

        if(FD_ISSET(sockfd, &read_fds)) {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            int new_client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);

            for(int i = 0; i < MAX_CLIENTS; i++) {
                if(clients[i].fd == -1) {
                    clients[i].fd = new_client_fd;
                    memset(clients[i].line_buf, 0, sizeof(clients[i].line_buf));
                    memset(clients[i].portfolio, 0, sizeof(clients[i].portfolio));
                    clients[i].portfolio_size = 0;
                    memset(clients[i].type, 0, sizeof(clients[i].type));
                    memset(clients[i].username, 0, sizeof(clients[i].username));

                    printf("[SERVER] CLIENT_CONNECTED fd=%d ip=%s\n", clients[i].fd, inet_ntoa(client_addr.sin_addr));
                    fprintf(log_fptr, "[SERVER] CLIENT_CONNECTED fd=%d ip=%s\n", clients[i].fd, inet_ntoa(client_addr.sin_addr));

                    break;
                }
            }
        }

        for(int i = 0; i < MAX_CLIENTS; i++) {
            if(clients[i].fd == -1) continue;

            if(FD_ISSET(clients[i].fd, &read_fds)) {
                char temp_buf[256]; 
                memset(temp_buf, 0, sizeof(temp_buf));

                int bytes_read = read(clients[i].fd, temp_buf, sizeof(temp_buf) - 1);
                if(bytes_read == 0) {
                    close(clients[i].fd);

                    printf("[SERVER] CLIENT_DISCONNECTED username=%s reason=hangup\n", clients[i].username);
                    fprintf(log_fptr, "[SERVER] CLIENT_DISCONNECTED username=%s reason=hangup\n", clients[i].username);

                    clients[i].fd = -1;
                    memset(clients[i].line_buf, 0, sizeof(clients[i].line_buf));
                    memset(clients[i].portfolio, 0, sizeof(clients[i].portfolio));
                    clients[i].portfolio_size = 0;
                    memset(clients[i].type, 0, sizeof(clients[i].type));
                    memset(clients[i].username, 0, sizeof(clients[i].username));

                    continue;
                }
                else if(bytes_read < 0) {
                    continue;
                }

                if(strlen(clients[i].line_buf) + bytes_read >= 512) {
                    send(clients[i].fd, "ERR TOOLONG\n", sizeof("ERR TOOLONG\n"), 0);
                    memset(clients[i].line_buf, 0, sizeof(clients[i].line_buf));

                    continue;
                }

                strcat(clients[i].line_buf, temp_buf);

                char* newline_ptr;

                while((newline_ptr = strchr(clients[i].line_buf, '\n')) != NULL) {
                    *newline_ptr = '\0';

                    handle_client_command(&clients[i], clients[i].line_buf, &global_state);

                    char* next_command_ptr = newline_ptr + 1;
                    int remaining_bytes = strlen(next_command_ptr);

                    if(remaining_bytes > 0) {
                        memmove(clients[i].line_buf, next_command_ptr, remaining_bytes + 1);
                    }
                    else {
                        memset(clients[i].line_buf, 0, sizeof(clients[i].line_buf));
                    }
                }
            }
        }
    }

    close(sockfd);
    close(u_sockfd);

    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(clients[i].fd != -1) {
            send(clients[i].fd, "SERVER SHUTDOWN\n", 17, 0);

            close(clients[i].fd);
        }
    }

    printf("\n[SERVER] SHUTDOWN signal=SIGINT\n");
    fprintf(log_fptr, "[SERVER] SHUTDOWN signal=SIGINT\n");

    printf("[SERVER] CLEANUP_DONE clients=0\n");
    fprintf(log_fptr, "[SERVER] CLEANUP_DONE clients=0\n");

    fclose(log_fptr);

    return 0;
}