#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <time.h>
#include <signal.h>
#include "defs.h"

static char* USAGE_MESSAGE  =  "./hogwarts -p <tcp_port> -s <ingredients.txt> -l <logfile> -n <max_clients> -t <timeout>\n";

void print_usage() {
    fprintf(stderr, "%s", USAGE_MESSAGE);
    exit(EXIT_FAILURE);
}

void handle_sigint(int sig) {
    (void)sig;
    stop_flag = 1;
}

int main(int argc, char *argv[]) {
    if(argc == 1) print_usage();

    int opt;
    int tcp_port = 0;
    char* ingredients = NULL;
    char* logfile = NULL;
    int max_clients = 0;
    int timeout = 0;

    while((opt = getopt(argc, argv, "p:s:l:n:t:")) != -1) {
        switch(opt) {
            case 'p': 
                tcp_port = atoi(optarg);
                break;
            case 's':
                ingredients = optarg;
                break;
            case 'l':
                logfile = optarg;
                break;
            case 'n':
                max_clients = atoi(optarg);
                break;
            case 't':
                timeout = atoi(optarg);
                break;
            default:
                print_usage();
        }
    }

    if(tcp_port < 1024 || ingredients == NULL || logfile == NULL || max_clients < 1 || timeout < 1) {
        print_usage();
    }

    signal(SIGINT, handle_sigint);

    int ingredients_read = read_ing_file(ingredients, NULL, 0);
    ingredient_t ings_array[ingredients_read];
    read_ing_file(ingredients, ings_array, 1);

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

    client_t clients[max_clients];
    for(int i = 0; i < max_clients; i++) {
        clients[i].fd = -1;
        memset(clients[i].username, 0, sizeof(clients[i].username));
        memset(clients[i].line_buf, 0, sizeof(clients[i].line_buf));
        clients[i].spellbook.count = 0;
    }

    FILE* log_fptr = fopen(logfile, "w");
    printf("Hogwarts is ready. Port: %d | Max Clients: %d | Timeout: %ds\n", tcp_port, max_clients, timeout);
    
    char ts_str[32];
    time_t startup_time = time(NULL);
    strftime(ts_str, sizeof(ts_str), "[%Y-%m-%d %H:%M:%S]", localtime(&startup_time));
    fprintf(log_fptr, "%s [SERVER] SERVER_STARTED port=%d max_clients=%d timeout=%d ingredients=%d\n", ts_str, tcp_port, max_clients, timeout, ingredients_read);
    printf("%s [SERVER] SERVER_STARTED port=%d max_clients=%d timeout=%d ingredients=%d\n", ts_str, tcp_port, max_clients, timeout, ingredients_read);
    fflush(log_fptr);

    while(!stop_flag) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        
        int max_fd = sockfd;
        time_t now = time(NULL);
        int min_remaining = timeout;
        
        for(int i = 0; i < max_clients; i++) {
            if(clients[i].fd != -1) {
                int elapsed = now - clients[i].last_active;

                if(elapsed >= timeout) {
                    send(clients[i].fd, "TIMEOUT DISCONNECT\n", 19, 0);
                    
                    char timestamp_str[32];
                    strftime(timestamp_str, sizeof(timestamp_str), "[%Y-%m-%d %H:%M:%S]", localtime(&now));
                    
                    fprintf(log_fptr, "%s [SERVER] TIMEOUT username=%s fd=%d elapsed=%ds\n", timestamp_str, clients[i].username, clients[i].fd, elapsed);
                    printf("%s [SERVER] TIMEOUT username=%s fd=%d elapsed=%ds\n", timestamp_str, clients[i].username, clients[i].fd, elapsed);

                    fprintf(log_fptr, "%s [SERVER] CLIENT_DISCONNECTED username=%s reason=timeout\n", timestamp_str, clients[i].username);
                    printf("%s [SERVER] CLIENT_DISCONNECTED username=%s reason=timeout\n", timestamp_str, clients[i].username);
                    
                    fflush(log_fptr);

                    close(clients[i].fd);
                    clients[i].fd = -1;
                    memset(clients[i].username, 0, sizeof(clients[i].username));
                    memset(clients[i].line_buf, 0, sizeof(clients[i].line_buf));
                    clients[i].spellbook.count = 0;
                    continue;
                }

                FD_SET(clients[i].fd, &read_fds);
                max_fd = clients[i].fd > max_fd ? clients[i].fd : max_fd;

                int remaining = timeout - elapsed;
                min_remaining = remaining < min_remaining ? remaining : min_remaining;
            }
        }

        struct timeval tv;
        tv.tv_sec = min_remaining;
        tv.tv_usec = 0;

        int activity = select(max_fd + 1, &read_fds, NULL, NULL, &tv);
        if(activity < 0) continue; 

        if(FD_ISSET(sockfd, &read_fds)) {
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);
            int new_worker_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);
            
            int current_clients = 0;
            for(int i = 0; i < max_clients; i++) {
                if(clients[i].fd != -1) current_clients++;
            }

            now = time(NULL);
            char timestamp_str[32];
            strftime(timestamp_str, sizeof(timestamp_str), "[%Y-%m-%d %H:%M:%S]", localtime(&now));

            if(current_clients >= max_clients) {
                send(new_worker_fd, "ERR HOGWARTS_FULL\n", 18, 0);
                fprintf(log_fptr, "%s [SERVER] REJECTED fd=%d ip=%s reason=HOGWARTS_FULL\n", timestamp_str, new_worker_fd, inet_ntoa(client_addr.sin_addr));
                printf("%s [SERVER] REJECTED fd=%d ip=%s reason=HOGWARTS_FULL\n", timestamp_str, new_worker_fd, inet_ntoa(client_addr.sin_addr));
                fflush(log_fptr);
                close(new_worker_fd);
            }
            else {
                int empty_index = 0;
                for(int i = 0; i < max_clients; i++) {
                    if(clients[i].fd == -1) {
                        empty_index = i;
                        break;
                    }
                }

                clients[empty_index].fd = new_worker_fd;
                clients[empty_index].last_active = time(NULL);
                memset(clients[empty_index].line_buf, 0, sizeof(clients[empty_index].line_buf));
                clients[empty_index].spellbook.count = 0;

                fprintf(log_fptr, "%s [SERVER] CLIENT_CONNECTED fd=%d ip=%s\n", timestamp_str, clients[empty_index].fd, inet_ntoa(client_addr.sin_addr));
                printf("%s [SERVER] CLIENT_CONNECTED fd=%d ip=%s\n", timestamp_str, clients[empty_index].fd, inet_ntoa(client_addr.sin_addr));
                fflush(log_fptr);
            }
        }

        for(int i = 0; i < max_clients; i++) {
            if(clients[i].fd != -1 && FD_ISSET(clients[i].fd, &read_fds)) {
                char temp_buf[256];
                int bytes_read = recv(clients[i].fd, temp_buf, sizeof(temp_buf) - 1, 0);
                
                now = time(NULL);
                char timestamp_str[32];
                strftime(timestamp_str, sizeof(timestamp_str), "[%Y-%m-%d %H:%M:%S]", localtime(&now));

                if(bytes_read <= 0) {
                    fprintf(log_fptr, "%s CLIENT_DISCONNECTED username=%s reason=hangup\n", timestamp_str, clients[i].username);
                    printf("%s CLIENT_DISCONNECTED username=%s reason=hangup\n", timestamp_str, clients[i].username);
                    fflush(log_fptr);
                    
                    close(clients[i].fd);
                    
                    clients[i].fd = -1;
                    memset(clients[i].line_buf, 0, sizeof(clients[i].line_buf));
                    memset(clients[i].username, 0, sizeof(clients[i].username));
                    clients[i].spellbook.count = 0;
                }
                else {
                    temp_buf[bytes_read] = '\0';
                    strcat(clients[i].line_buf, temp_buf);

                    char* pos;
                    while((pos = strchr(clients[i].line_buf, '\n')) != NULL) {
                        int len = pos - clients[i].line_buf;
                        char current_cmd[512];
                        strncpy(current_cmd, clients[i].line_buf, len);
                        current_cmd[len] = '\0';

                        memmove(clients[i].line_buf, pos + 1, strlen(pos + 1) + 1);

                        char tokens[3][32];
                        memset(tokens, 0, sizeof(tokens));
                        char* token = strtok(current_cmd, " ");
                        int tok_index = 0;

                        while(token != NULL && tok_index < 3) {
                            strcpy(tokens[tok_index], token);
                            tok_index++;
                            token = strtok(NULL, " ");
                        }

                        char send_buf[512];

                        if(strcmp("ENROLL", tokens[0]) != 0 && clients[i].username[0] == '\0') {
                            send(clients[i].fd, "ERR NOT_ENROLLED\n", 17, 0);
                            continue;
                        }

                        if(strcmp("ENROLL", tokens[0]) == 0) {
                            if(tokens[1][0] == '\0' || tokens[2][0] == '\0') {
                                send(clients[i].fd, "ERR UNKNOWN\n", 12, 0);
                                continue;
                            }
                            if(strcmp(tokens[1], "PROFESSOR") != 0 && strcmp(tokens[1], "WIZARD") != 0) {
                                send(clients[i].fd, "ERR UNKNOWN <type>\n", 19, 0);
                                continue;
                            }
                            int taken = 0;
                            for(int j = 0; j < max_clients; j++) {
                                if(clients[j].fd != -1 && strcmp(tokens[2], clients[j].username) == 0) {
                                    taken = 1;
                                    break;
                                }
                            }
                            if(taken) {
                                send(clients[i].fd, "ERR ENROLL name_taken\n", 22, 0);
                                continue;
                            }

                            strcpy(clients[i].type, tokens[1]);
                            strcpy(clients[i].username, tokens[2]);
                            sprintf(send_buf, "OK ENROLL %s\n", clients[i].username);
                            send(clients[i].fd, send_buf, strlen(send_buf), 0);

                            fprintf(log_fptr, "%s [SERVER] ENROLL username=%s type=%s fd=%d\n", timestamp_str, clients[i].username, clients[i].type, clients[i].fd);
                            printf("%s [SERVER] ENROLL username=%s type=%s fd=%d\n", timestamp_str, clients[i].username, clients[i].type, clients[i].fd);
                            fflush(log_fptr);
                        }
                        else if(strcmp("BREW", tokens[0]) == 0 || strcmp("CONSUME", tokens[0]) == 0) {
                            if(strcmp(clients[i].type, "WIZARD") != 0) {
                                send(clients[i].fd, "ERR UNAUTHORIZED\n", 17, 0);
                                continue;
                            }

                            int found_index = -1;
                            for(int j = 0; j < ingredients_read; j++) {
                                if(strcmp(ings_array[j].name, tokens[1]) == 0) {
                                    found_index = j;
                                    break;
                                }
                            }

                            if(found_index == -1) {
                                send(clients[i].fd, "ERR UNKNOWN_INGREDIENT\n", 23, 0);
                                continue;
                            }

                            int qty = atoi(tokens[2]);
                            int old_qty = ings_array[found_index].quantity;

                            if(strcmp("BREW", tokens[0]) == 0) {
                                ings_array[found_index].quantity += qty;
                                
                                int sb_found = 0;
                                for(int k = 0; k < clients[i].spellbook.count; k++) {
                                    if(strcmp(clients[i].spellbook.items[k].name, tokens[1]) == 0) {
                                        clients[i].spellbook.items[k].quantity += qty;
                                        sb_found = 1;
                                        break;
                                    }
                                }
                                if(!sb_found) {
                                    strcpy(clients[i].spellbook.items[clients[i].spellbook.count].name, tokens[1]);
                                    clients[i].spellbook.items[clients[i].spellbook.count].quantity = qty;
                                    clients[i].spellbook.count++;
                                }

                                sprintf(send_buf, "OK BREW %s %d %d\n", tokens[1], qty, ings_array[found_index].quantity);
                                send(clients[i].fd, send_buf, strlen(send_buf), 0);
                                fprintf(log_fptr, "%s [SERVER] BREW wizard=%s ingredient=%s qty=%d old_qty=%d new_qty=%d\n", timestamp_str, clients[i].username, tokens[1], qty, old_qty, ings_array[found_index].quantity);
                                printf("%s [SERVER] BREW wizard=%s ingredient=%s qty=%d old_qty=%d new_qty=%d\n", timestamp_str, clients[i].username, tokens[1], qty, old_qty, ings_array[found_index].quantity);
                            } 
                            else {
                                int sb_qty = 0;
                                int sb_idx = -1;
                                for(int k = 0; k < clients[i].spellbook.count; k++) {
                                    if(strcmp(clients[i].spellbook.items[k].name, tokens[1]) == 0) {
                                        sb_qty = clients[i].spellbook.items[k].quantity;
                                        sb_idx = k;
                                        break;
                                    }
                                }

                                if(old_qty < qty || sb_qty < qty) {
                                    send(clients[i].fd, "ERR INSUFFICIENT_INGREDIENTS\n", 29, 0);
                                    continue;
                                }

                                ings_array[found_index].quantity -= qty;
                                clients[i].spellbook.items[sb_idx].quantity -= qty;

                                sprintf(send_buf, "OK CONSUME %s %d %d\n", tokens[1], qty, ings_array[found_index].quantity);
                                send(clients[i].fd, send_buf, strlen(send_buf), 0);
                                fprintf(log_fptr, "%s [SERVER] CONSUME wizard=%s ingredient=%s qty=%d old_qty=%d new_qty=%d\n", timestamp_str, clients[i].username, tokens[1], qty, old_qty, ings_array[found_index].quantity);
                                printf("%s [SERVER] CONSUME wizard=%s ingredient=%s qty=%d old_qty=%d new_qty=%d\n", timestamp_str, clients[i].username, tokens[1], qty, old_qty, ings_array[found_index].quantity);
                            }
                            fflush(log_fptr);
                        }
                        else if(strcmp("SPELLBOOK", tokens[0]) == 0) {
                            if(strcmp(clients[i].type, "WIZARD") != 0) {
                                send(clients[i].fd, "ERR UNAUTHORIZED\n", 17, 0);
                                continue;
                            }
                            char sb_buf[512] = "OK SPELLBOOK ";
                            int added = 0;
                            for(int k = 0; k < clients[i].spellbook.count; k++) {
                                if(clients[i].spellbook.items[k].quantity > 0) {
                                    char item_str[32];
                                    sprintf(item_str, "%s%s:%d", (added > 0 ? "," : ""), clients[i].spellbook.items[k].name, clients[i].spellbook.items[k].quantity);
                                    strcat(sb_buf, item_str);
                                    added++;
                                }
                            }
                            if(added == 0) strcat(sb_buf, "EMPTY");
                            strcat(sb_buf, "\n");
                            send(clients[i].fd, sb_buf, strlen(sb_buf), 0);
                        }
                        else if(strcmp("INSPECT", tokens[0]) == 0) {
                            int found_index = -1;
                            for(int j = 0; j < ingredients_read; j++) {
                                if(strcmp(ings_array[j].name, tokens[1]) == 0) {
                                    found_index = j;
                                    break;
                                }
                            }
                            if(found_index == -1) {
                                send(clients[i].fd, "ERR UNKNOWN_INGREDIENT\n", 23, 0);
                            } else {
                                sprintf(send_buf, "OK INSPECT %s %d\n", tokens[1], ings_array[found_index].quantity);
                                send(clients[i].fd, send_buf, strlen(send_buf), 0);
                                fprintf(log_fptr, "%s [SERVER] INSPECT professor=%s ingredient=%s qty=%d\n", timestamp_str, clients[i].username, tokens[1], ings_array[found_index].quantity);
                                printf("%s [SERVER] INSPECT professor=%s ingredient=%s qty=%d\n", timestamp_str, clients[i].username, tokens[1], ings_array[found_index].quantity);
                                fflush(log_fptr);
                            }
                        }
                        else if(strcmp("SCROLL", tokens[0]) == 0) {
                            if(strcmp(clients[i].type, "PROFESSOR") != 0) {
                                send(clients[i].fd, "ERR UNAUTHORIZED\n", 17, 0);
                                continue;
                            }
                            char scr_buf[512] = "OK SCROLL ";
                            for(int j = 0; j < ingredients_read; j++) {
                                char item_str[32];
                                sprintf(item_str, "%s%s:%d", (j > 0 ? "," : ""), ings_array[j].name, ings_array[j].quantity);
                                strcat(scr_buf, item_str);
                            }
                            strcat(scr_buf, "\n");
                            send(clients[i].fd, scr_buf, strlen(scr_buf), 0);
                            fprintf(log_fptr, "%s [SERVER] SCROLL professor=%s ingredients=%d\n", timestamp_str, clients[i].username, ingredients_read);
                            printf("%s [SERVER] SCROLL professor=%s ingredients=%d\n", timestamp_str, clients[i].username, ingredients_read);
                            fflush(log_fptr);
                        }
                        else if(strcmp("ROSTER", tokens[0]) == 0) {
                            if(strcmp(clients[i].type, "PROFESSOR") != 0) {
                                send(clients[i].fd, "ERR UNAUTHORIZED\n", 17, 0);
                                continue;
                            }
                            char ros_buf[512] = "OK ROSTER ";
                            int added = 0;
                            for(int j = 0; j < max_clients; j++) {
                                if(clients[j].fd != -1 && clients[j].username[0] != '\0') {
                                    char usr_str[32];
                                    sprintf(usr_str, "%s%s", (added > 0 ? "," : ""), clients[j].username);
                                    strcat(ros_buf, usr_str);
                                    added++;
                                }
                            }
                            strcat(ros_buf, "\n");
                            send(clients[i].fd, ros_buf, strlen(ros_buf), 0);
                            fprintf(log_fptr, "%s [SERVER] ROSTER professor=%s clients=%d\n", timestamp_str, clients[i].username, added);
                            printf("%s [SERVER] ROSTER professor=%s clients=%d\n", timestamp_str, clients[i].username, added);
                            fflush(log_fptr);
                        }
                        else if(strcmp("APPARATE", tokens[0]) == 0) {
                            send(clients[i].fd, "OK APPARATE\n", 12, 0);
                            fprintf(log_fptr, "%s [SERVER] CLIENT_DISCONNECTED username=%s reason=APPARATE\n", timestamp_str, clients[i].username);
                            printf("%s [SERVER] CLIENT_DISCONNECTED username=%s reason=APPARATE\n", timestamp_str, clients[i].username);
                            fflush(log_fptr);
                            
                            close(clients[i].fd);
                            
                            clients[i].fd = -1;
                            memset(clients[i].line_buf, 0, sizeof(clients[i].line_buf));
                            memset(clients[i].username, 0, sizeof(clients[i].username));
                            clients[i].spellbook.count = 0;
                        }
                        else {
                            send(clients[i].fd, "ERR UNKNOWN\n", 12, 0);
                        }
                        
                        if(clients[i].fd != -1) clients[i].last_active = time(NULL);
                    }
                }
            }
        }
    }

    time_t shutdown_time = time(NULL);
    char s_ts_str[32];
    strftime(s_ts_str, sizeof(s_ts_str), "[%Y-%m-%d %H:%M:%S]", localtime(&shutdown_time));
    fprintf(log_fptr, "%s [SERVER] SHUTDOWN signal=SIGINT\n", s_ts_str);
    printf("%s [SERVER] SHUTDOWN signal=SIGINT\n", s_ts_str);

    fprintf(log_fptr, "%s [SERVER] CLEANUP_DONE clients=0\n", s_ts_str);
    printf("%s [SERVER] CLEANUP_DONE clients=0\n", s_ts_str);
    
    fflush(log_fptr);

    for(int i = 0; i < max_clients; i++) {
        if(clients[i].fd != -1) {
            send(clients[i].fd, "SERVER SHUTDOWN\n", 16, 0);
            close(clients[i].fd);
        }
    }
    
    close(sockfd);
    fclose(log_fptr);
    return 0;
}
