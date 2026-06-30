#pragma once
#include <signal.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 16
#define MAX_ITEMS 32

extern volatile sig_atomic_t stop_flag;

typedef struct {
    char symbol[9];
    int quantity;
} portfolio_item_t;

typedef struct {
    int fd;
    char username[32];
    char type[8];
    char line_buf[512];
    
    portfolio_item_t portfolio[MAX_ITEMS];
    int portfolio_size;
} client_t;

typedef struct {
    char symbol[9];
    float initial_price;
} stock_t;

typedef struct {
    client_t* clients;
    stock_t* stocks;
    int num_stocks;
    int u_sockfd;
    FILE* log_fptr;
    struct sockaddr_in bcast_addr;
} server_state_t;

int read_stock_file(char* filename, stock_t* array, int mode);
void broadcast_all_prices(server_state_t *state, const char *trigger);
void handle_client_command(client_t *current_client, char *command, server_state_t *state);