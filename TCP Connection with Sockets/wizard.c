#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include "defs.h"


char* USAGE_MESSAGE  =  "./wizard <server_ip> <tcp_port> <username>\n"
                        "\nConstraints:"
                        "\n Server ip. Must be > 0 probably"
                        "\n TCP port for wizard and professor connections. Must be >= 1024"
                        "\n Username must be < 32 characters\n";

void handle_sigint(int sig) {
    (void)sig;
    stop_flag = 1;
}

void print_usage() {
    fprintf(stderr, "%s", USAGE_MESSAGE);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if(argc != 4) {
        print_usage();
    }

    char* server_ip = argv[1];
    int tcp_port = atoi(argv[2]);
    char* username = argv[3];

    if(server_ip == NULL || tcp_port < 1024 || username == NULL || strlen(username) > 31) {
        fprintf(stderr, "Missing/invalid parameters.\n");
        print_usage();
    }

    signal(SIGINT, handle_sigint);
    signal(SIGPIPE, SIG_IGN);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(sockfd < 0) {
        perror("Could not create socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(tcp_port);

    if(inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address\n");
        exit(EXIT_FAILURE);
    }

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Unable to connect");
        exit(EXIT_FAILURE);
    }
    
    char buf[512];
    sprintf(buf, "ENROLL WIZARD %s\n", username);
    send(sockfd, buf, strlen(buf), 0);
    printf("[WIZARD %s] CONNECTED server=%s:%d\n", username, server_ip, tcp_port);

    char client_line_buf[1024] = "";

    while(!stop_flag) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(sockfd, &read_fds);

        int activity = select(sockfd + 1, &read_fds, NULL, NULL, NULL);
        if(activity < 0) continue;

        if(FD_ISSET(STDIN_FILENO, &read_fds)) {
            if(fgets(buf, sizeof(buf), stdin) != NULL) {
                send(sockfd, buf, strlen(buf), 0);
                buf[strcspn(buf, "\n")] = 0;
                printf("[WIZARD %s] SENT %s\n", username, buf);
            }
        }

        if(FD_ISSET(sockfd, &read_fds)) {
            char temp_buf[256];
            int bytes_read = recv(sockfd, temp_buf, sizeof(temp_buf) - 1, 0);
            if(bytes_read <= 0) {
                printf("[WIZARD %s] DISCONNECTED reason=hangup\n", username);
                break;
            }
            temp_buf[bytes_read] = '\0';

            strcat(client_line_buf, temp_buf);

            char* pos;
            while((pos = strchr(client_line_buf, '\n')) != NULL) {
                int len = pos - client_line_buf;
                char current_line[512];
                strncpy(current_line, client_line_buf, len);
                current_line[len] = '\0';

                memmove(client_line_buf, pos + 1, strlen(pos + 1) + 1);

                printf("[WIZARD %s] RECEIVED %s\n", username, current_line);

                if(strcmp(current_line, "OK APPARATE") == 0 || 
                strcmp(current_line, "SERVER SHUTDOWN") == 0 || 
                strcmp(current_line, "TIMEOUT DISCONNECT") == 0) {
                    stop_flag = 1;
                }
            }
        }
    }

    if(stop_flag) {
        send(sockfd, "APPARATE\n", 9, 0);
    }
    
    close(sockfd);
    return 0;
}