#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "defs.h"

char* USAGE_MESSAGE  =  "./ticker <udp_port>";

void print_usage() {
    fprintf(stderr, "%s\n", USAGE_MESSAGE);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if(argc != 2) print_usage();

    int udp_port = atoi(argv[1]);

    if(udp_port < 1024) {
        fprintf(stderr, "Missing or invalid parameter. Please obey the correct usage.\n");
        print_usage();
    }

    int u_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(u_sockfd < 0) exit(EXIT_FAILURE);

    int opt_val = 1;
    
    setsockopt(u_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
    
    #ifdef SO_REUSEPORT
    setsockopt(u_sockfd, SOL_SOCKET, SO_REUSEPORT, &opt_val, sizeof(opt_val));
    #endif

    struct sockaddr_in u_server_addr;
    memset(&u_server_addr, 0, sizeof(u_server_addr));
    u_server_addr.sin_family = AF_INET;
    u_server_addr.sin_addr.s_addr = INADDR_ANY;
    u_server_addr.sin_port = htons(udp_port);

    if(bind(u_sockfd, (struct sockaddr *)&u_server_addr, sizeof(u_server_addr)) < 0) exit(EXIT_FAILURE);

    printf("[TICKER] LISTENING udp_port=%d\n", udp_port);

    char buffer[512];

    while(1) {
        int bytes_read = recvfrom(u_sockfd, buffer, sizeof(buffer) - 1, 0, NULL, NULL);

        if(bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("[TICKER] RECEIVED %s", buffer);
            fflush(stdout);
        }
    }
    return 0;
}