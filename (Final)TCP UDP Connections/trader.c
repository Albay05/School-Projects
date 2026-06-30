#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include "defs.h"

char* USAGE_MESSAGE  =  "./trader <server_ip> <tcp_port> <username>";

void print_usage() {
    fprintf(stderr, "%s\n", USAGE_MESSAGE);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if(argc != 4) print_usage();

    char* server_ip = argv[1];
    int tcp_port = atoi(argv[2]);
    char* username = argv[3];

    if(server_ip == NULL || tcp_port < 1024 || username == NULL) {
        fprintf(stderr, "Missing or invalid parameter. Please obey the correct usage.\n");
        print_usage();
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(tcp_port);
    inet_pton(AF_INET, server_ip, &serv_addr.sin_addr);

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Unable to connect");
        exit(EXIT_FAILURE);
    }

    printf("[TRADER %s] CONNECTED server=%s:%d\n", username, server_ip, tcp_port);

    char buf[512];
    sprintf(buf, "JOIN TRADER %s\n", username);
    send(sockfd, buf, strlen(buf), 0);

    printf("[TRADER %s] SENT JOIN\n", username);

    while(1) {
        memset(buf, 0, sizeof(buf));

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        FD_SET(sockfd, &read_fds);

        int max_fd = (sockfd > STDIN_FILENO) ? sockfd : STDIN_FILENO;

        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if(activity < 0) {
            perror("select error");
            exit(EXIT_FAILURE);
        }
        
        if(FD_ISSET(STDIN_FILENO, &read_fds)) {
            if(fgets(buf, sizeof(buf), stdin) != NULL) {
                send(sockfd, buf, strlen(buf), 0);
                buf[strcspn(buf, "\n")] = 0;
                printf("[TRADER %s] SENT %s\n", username, buf);
            }
            else {
                send(sockfd, "QUIT\n", sizeof("QUIT\n"), 0);
                printf("[TRADER %s] DISCONNECTED reason=QUIT\n", username);

                close(sockfd);
                break;
            }
        }
        if(FD_ISSET(sockfd, &read_fds)) {
            int bytes_read = read(sockfd, buf, sizeof(buf) - 1);
            if(bytes_read > 0) {
                buf[bytes_read] = '\0';
                buf[strcspn(buf, "\n")] = 0;
            }

            if(bytes_read == 0 || strncmp(buf, "SERVER SHUTDOWN", 15) == 0) {
                printf("[TRADER %s] DISCONNECTED reason=shutdown\n", username);

                close(sockfd);
                break;
            }

            printf("[TRADER %s] RECEIVED %s\n", username, buf);
        }
    }
    return 0;
}