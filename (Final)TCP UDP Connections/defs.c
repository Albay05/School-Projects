#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include "defs.h"

int read_stock_file(char* filename, stock_t* array, int mode) {
    if(!filename) {
        fprintf(stderr, "Error opening ingredient file");
        exit(EXIT_FAILURE);
    }

    FILE *fptr = fopen(filename, "r");
    if(!fptr) {
        return 0;
    }
    
    char line[64];
    int valid_lines = 0;

    while(fgets(line, sizeof(line), fptr)) {
        char sym[9];
        float pri;
        if(sscanf(line, "%8s %f", sym, &pri) == 2) {
            if(pri > 0) {
                if(mode == 1) {
                    strcpy(array[valid_lines].symbol, sym);
                    array[valid_lines].initial_price = pri;
                }
                valid_lines++;
            }
            else continue;
        }
        else continue;
    }

    fclose(fptr);
    return valid_lines;
}

void broadcast_all_prices(server_state_t *state, const char *trigger) {
    char price_message[512];

    int offset = sprintf(price_message, "PRICE_UPDATE ");
    for(int i = 0; i < state->num_stocks; i++) {
        offset += sprintf(price_message + offset, "%s:%.2f ", state->stocks[i].symbol, state->stocks[i].initial_price);
    }
    sprintf(price_message + offset, "\n");

    sendto(state->u_sockfd, price_message, strlen(price_message), 0, (struct sockaddr *)&state->bcast_addr, sizeof(state->bcast_addr));

    fprintf(state->log_fptr, "[SERVER] PRICE_BROADCAST trigger=%s\n", trigger);
    printf("[SERVER] PRICE_BROADCAST trigger=%s\n", trigger);
    fflush(state->log_fptr);
}

void handle_client_command(client_t *current_client, char *command, server_state_t *state) {
    int len = strlen(command);
    char current_cmd[512];
    strncpy(current_cmd, command, len);
    current_cmd[len] = '\0';

    char tokens[3][32];
    memset(tokens, 0, sizeof(tokens));
    char* token = strtok(current_cmd, " ");
    int tok_index = 0;

    while(token != NULL && tok_index < 3) {
        strncpy(tokens[tok_index], token, 31);
        tokens[tok_index][31] = '\0';
        tok_index++;
        token = strtok(NULL, " ");
    }

    char send_buf[512];

    if(strcmp("JOIN", tokens[0]) != 0 && current_client->username[0] == '\0') {
        send(current_client->fd, "ERR NOT_JOINED\n", sizeof("ERR NOT_JOINED\n"), 0);
        return;
    }

    if(strcmp("JOIN", tokens[0]) == 0) {
        if(tokens[1][0] == '\0' || tokens[2][0] == '\0') {
            send(current_client->fd, "ERR UNKNOWN\n", 12, 0);
            return;
        }
        if(strcmp(tokens[1], "TRADER") != 0 && strcmp(tokens[1], "ANALYST") != 0) {
            send(current_client->fd, "ERR UNKNOWN <type>\n", 19, 0);
            return;
        }
        int taken = 0;
        for(int i = 0; i < MAX_CLIENTS; i++) {
            if(state->clients[i].fd != -1 && strcmp(tokens[2], state->clients[i].username) == 0) {
                taken = 1;
                break;
            }
        }
        if(taken) {
            send(current_client->fd, "ERR JOIN name_taken\n", 22, 0);
            return;
        }

        strcpy(current_client->type, tokens[1]);
        strcpy(current_client->username, tokens[2]);
        sprintf(send_buf, "OK JOIN %s\n", current_client->username);
        send(current_client->fd, send_buf, strlen(send_buf), 0);

        fprintf(state->log_fptr, "[SERVER] JOIN username=%s type=%s fd=%d\n", current_client->username, current_client->type, current_client->fd);
        printf("[SERVER] JOIN username=%s type=%s fd=%d\n", current_client->username, current_client->type, current_client->fd);
        fflush(state->log_fptr);
    }
    else if(strcmp("BUY", tokens[0]) == 0 || strcmp("SELL", tokens[0]) == 0 || strcmp("PORTFOLIO", tokens[0]) == 0) {
        if(strcmp(current_client->type, "TRADER") != 0) {
            send(current_client->fd, "ERR UNAUTHORIZED\n", 18, 0);
            return;
        }

        if(strcmp(tokens[0], "PORTFOLIO") == 0) {
            char message[1024] = "OK PORTFOLIO ";
            int start_len = strlen(message);

            for(int i = 0; i < MAX_ITEMS; i++) {
                if(i == 0 && current_client->portfolio[i].symbol[0] == '\0') {
                    strcat(message, "EMPTY");
                    break;
                }
                if(current_client->portfolio[i].symbol[0] != '\0') {
                    char entity[32];
                    sprintf(entity, "%s:%d,", current_client->portfolio[i].symbol, current_client->portfolio[i].quantity);
                    strcat(message, entity);
                }
                else {
                    break;
                }
            }
            int len = strlen(message);
            if(len > start_len && message[len - 1] == ',') {
                message[len - 1] = '\0';
            }
            strcat(message, "\n");
            send(current_client->fd, message, strlen(message), 0);
        }
        else {
            if(tokens[1][0] == '\0' || tokens[2][0] == '\0') {
                send(current_client->fd, "ERR UNKNOWN\n", 13, 0);
                return;
            }

            int index_found = -1;
            for(int i = 0; i < state->num_stocks; i++) {
                if(strcmp(state->stocks[i].symbol, tokens[1]) == 0) {
                    index_found = i;
                    break;
                }
            }

            if(index_found == -1) {
                send(current_client->fd, "ERR UNKNOWN_SYMBOL\n", 20, 0);
                return;
            }
            int quantity = atoi(tokens[2]);

            if(strcmp(tokens[0], "BUY") == 0) {
                float old_price = state->stocks[index_found].initial_price;
                state->stocks[index_found].initial_price += quantity * 0.01;

                char message[64];
                sprintf(message, "OK BUY %s %d %.2f\n", tokens[1], quantity, state->stocks[index_found].initial_price);

                broadcast_all_prices(state, "TRADE");

                int portfolio_index = -1;
                for(int i = 0; i < current_client->portfolio_size; i++) {
                    if(strcmp(tokens[1], current_client->portfolio[i].symbol) == 0) {
                        portfolio_index = i;
                        break;
                    }
                }
                if(portfolio_index == -1) {
                    portfolio_index = current_client->portfolio_size;
                    strcpy(current_client->portfolio[current_client->portfolio_size].symbol, tokens[1]);

                    current_client->portfolio_size++;
                }
                current_client->portfolio[portfolio_index].quantity += quantity;

                send(current_client->fd, message, strlen(message), 0);
                // sendto(state->u_sockfd, );

                fprintf(state->log_fptr, "[SERVER] BUY trader=%s symbol=%s qty=%d old_price=%.2f new_price=%.2f\n",
                        current_client->username, state->stocks[index_found].symbol, quantity, old_price, state->stocks[index_found].initial_price);
                printf("[SERVER] BUY trader=%s symbol=%s qty=%d old_price=%.2f new_price=%.2f\n",
                        current_client->username, state->stocks[index_found].symbol, quantity, old_price, state->stocks[index_found].initial_price);
                fflush(state->log_fptr);
            }
            else { // SELL
                int portfolio_index = -1;
                for(int i = 0; i < current_client->portfolio_size; i++) {
                    if(strcmp(current_client->portfolio[i].symbol, tokens[1]) == 0) {
                        portfolio_index = i;
                        break;
                    }
                }
                if(portfolio_index == -1 ||  quantity > current_client->portfolio[portfolio_index].quantity) {
                    send(current_client->fd, "ERR INSUFFICIENT_SHARES\n", sizeof("ERR INSUFFICIENT_SHARES\n"), 0);
                    return;
                }

                float old_price = state->stocks[index_found].initial_price;
                float new_price = old_price - quantity * 0.01;
                if(new_price < 0.01) new_price = 0.01;
                state->stocks[index_found].initial_price = new_price;

                current_client->portfolio[portfolio_index].quantity -= quantity;

                char message[64];
                sprintf(message, "OK SELL %s %d %.2f\n", tokens[1], quantity, state->stocks[index_found].initial_price);

                send(current_client->fd, message, strlen(message), 0);

                broadcast_all_prices(state, "TRADE");

                fprintf(state->log_fptr, "[SERVER] SELL trader=%s symbol=%s qty=%d old_price=%.2f new_price=%.2f\n",
                        current_client->username, state->stocks[index_found].symbol, quantity, old_price, state->stocks[index_found].initial_price);
                printf("[SERVER] SELL trader=%s symbol=%s qty=%d old_price=%.2f new_price=%.2f\n",
                        current_client->username, state->stocks[index_found].symbol, quantity, old_price, state->stocks[index_found].initial_price);
                fflush(state->log_fptr);
            }
        }
    }
    else if(strcmp("PRICE", tokens[0]) == 0 || strcmp("REPORT", tokens[0]) == 0 || strcmp("LIST", tokens[0]) == 0) {
        if(strcmp(current_client->type, "ANALYST") != 0) {
            send(current_client->fd, "ERR UNAUTHORIZED\n", 18, 0);
            return;
        }

        if(strcmp(tokens[0], "PRICE") == 0) {
            int index_found = -1;
            for(int i = 0; i < state->num_stocks; i++) {
                if(strcmp(state->stocks[i].symbol, tokens[1]) == 0) {
                    index_found = i;
                    break;
                }
            }

            if(index_found == -1) {
                send(current_client->fd, "ERR UNKNOWN_SYMBOL\n", 20, 0);
                return;
            }

            char message[32];
            sprintf(message, "OK PRICE %s %.2f\n", state->stocks[index_found].symbol, state->stocks[index_found].initial_price);

            send(current_client->fd, message, strlen(message), 0);
        }
        else if(strcmp("REPORT", tokens[0]) == 0) {
            char report_message[512];
            int offset = 0;

            for(int i = 0; i < state->num_stocks; i++) {
                offset += sprintf(report_message + offset, "%s:%.2f ", state->stocks[i].symbol, state->stocks[i].initial_price);
            }
            char result_message[524];
            sprintf(result_message, "OK REPORT %s\n", report_message);
            
            send(current_client->fd, result_message, strlen(result_message), 0);
        }
        else { // LIST
            char list[1024];
            int offset = sprintf(list, "OK LIST ");
            int start_offset = offset;

            for(int i = 0; i < MAX_CLIENTS; i++) {
                if(state->clients[i].fd == -1) {
                    continue;
                }
                offset += sprintf(list + offset, "%s,", state->clients[i].username);
            }

            if(offset > start_offset) offset--;

            sprintf(list + offset, "\n");

            send(current_client->fd, list, strlen(list), 0);
        }
    }
    else if(strcmp("QUIT", tokens[0]) == 0) {
        send(current_client->fd, "OK QUIT\n", sizeof("OK QUIT\n"), 0);

        printf("[SERVER] CLIENT_DISCONNECTED username=%s reason=QUIT\n", current_client->username);
        fprintf(state->log_fptr ,"[SERVER] CLIENT_DISCONNECTED username=%s reason=QUIT\n", current_client->username);

        close(current_client->fd);
        
        current_client->fd = -1;
        memset(current_client->line_buf, 0, sizeof(current_client->line_buf));
        memset(current_client->type, 0, sizeof(current_client->type));
        memset(current_client->username, 0, sizeof(current_client->username));
        memset(current_client->portfolio, 0, sizeof(current_client->portfolio));
        current_client->portfolio_size = 0;
    }
    else {
        send(current_client->fd, "ERR UNKNOWN\n", 13, 0);
    }
}