#define _XOPEN_SOURCE 700
#include "simulation.h"
#include "priority_queue.h"
#include "courier.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t global_shutdown_flag = 0;
volatile atomic_int completed_orders = 0;
volatile atomic_int cancelled_orders = 0;
volatile atomic_int total_delivery_time = 0;

char* USAGE_MESSAGE  =  "./cargoGTU -n <num_couriers> -i <orders.txt> -s <stats.txt>\n"
                        "\n\nConstraints:"
                        "\n -n  Number of courier threads in the pool(>=1)"
                        "\n -i  Path to input file"
                        "\n -s  Path to statistics output file\n";


void sigint_handler(int signum) {
    (void)signum;
    global_shutdown_flag = 1;
}

void read_file(char* filename, PriorityQueue *queue) {
    FILE* fptr = fopen(filename, "r");
    if(!fptr) {
        fprintf(stderr, "Error opening input file");
        exit(EXIT_FAILURE);
    }

    char line[256];

    while(fgets(line, sizeof(line),fptr)) {
        line[strcspn(line, "\r\n")] = 0;

        if (strlen(line) == 0) continue;

        order_t order;
        memset(&order, 0, sizeof(order));

        if(parse_line(line, &order) == 0) {
            Node* node = malloc(sizeof(Node));
            node->order = order;

            insert2queue(queue, node);

            printf("[CARGOGTU] ORDER_QUEUED id=%d recipient=%s priority=%s duration=%d\n",
                                            node->order.id,
                                            node->order.receiver_name,
                                            priority2string(node->order.priority_level),
                                            node->order.duration * 500);
        }
    }

    fclose(fptr);
}

int parse_line(char line[], order_t* order) {
    char pri_level_str[16];

    if(line == NULL) return -1;

    int count = sscanf(line, "%d %31s %15s %d", 
                       &order->id,
                       order->receiver_name,
                       pri_level_str,
                       &order->duration);

    if(count < 4) return -1;

    if(strcmp(pri_level_str, "EXPRESS") == 0) {
        order->priority_level = EXPRESS;
    }
    else if(strcmp(pri_level_str, "STANDARD") == 0) {
        order->priority_level = STANDARD;
    }
    else if(strcmp(pri_level_str, "ECONOMY") == 0) {
        order->priority_level = ECONOMY;
    }
    else
        return -1;

    return 0;
}

void run_simulation(int num_couriers, char * input_file, char * output_file) {
    PriorityQueue queue;
    queue.head = NULL;
    queue.size = 0;

    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    read_file(input_file, &queue);
    int total_orders_read = queue.size;
    
    SimulationManager sm;

    sm.num_couriers = num_couriers;
    
    pthread_mutexattr_t m_attr;
    pthread_mutexattr_init(&m_attr);
    pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);

    pthread_condattr_t c_attr;
    pthread_condattr_init(&c_attr);
    pthread_condattr_setpshared(&c_attr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&sm.queue_mutex, &m_attr);
    pthread_mutex_init(&sm.log_mutex, &m_attr);

    pthread_cond_init(&sm.queue_cond, &c_attr);

    pthread_t threads[num_couriers];

    CourierArgs args_array[num_couriers];

    printf("[CARGOGTU] SHIFT_START couriers=%d orders=%d\n",
                                    num_couriers,
                                    total_orders_read);

    for(int i = 0; i < num_couriers; i++) {
        args_array[i].courier_id = i + 1;
        args_array[i].shared_queue = &queue;
        args_array[i].sm = &sm;
        args_array[i].completed_count = 0;
        args_array[i].total_time = 0;

        pthread_create(&threads[i], NULL, courier_routine,(void*)&args_array[i]);
    }

    while(1) {
        if(global_shutdown_flag) {
            printf("[CARGOGTU] SIGINT RECEIVED pending_orders=%d\n", queue.size);
            
            pthread_cond_broadcast(&sm.queue_cond);
            break; 
        }

        if(completed_orders == total_orders_read) {
            global_shutdown_flag = 1;
            pthread_cond_broadcast(&sm.queue_cond);
            break;
        }
        usleep(100000); // 100ms
    }

    for(int i = 0; i < num_couriers; i++) {
        pthread_join(threads[i], NULL);
    }
    
    Node* remaining_order;
    while((remaining_order = pull_from_queue(&queue)) != NULL) {
        printf("[CARGOGTU] ORDER_CANCELLED id=%d recipient=%s priority%s\n",
                                            remaining_order->order.id,
                                            remaining_order->order.receiver_name,
                                            priority2string(remaining_order->order.priority_level));

        cancelled_orders++;
        free(remaining_order);
    }

    FILE* fptr = fopen(output_file, "w");
    if(!fptr) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }

    fprintf(fptr, "SHIFT_SUMMARY\n");
    fprintf(fptr, "Total orders : %d", total_orders_read);
    fprintf(fptr, "Completed    : %d", (int)completed_orders);
    fprintf(fptr, "Cancelled    : %d", (int)cancelled_orders);
    fprintf(fptr, "Total time   : %d", (int)total_delivery_time);
    fprintf(fptr, "Avg per order    : %d", (int)completed_orders > 0 ? (int)total_delivery_time * 500 / (int)completed_orders : 0);
    fprintf(fptr, "\nCOURIER_STATS\n");
    for(int i = 0; i < num_couriers; i++) {
        fprintf(fptr, "Courier-%d completed=%d total_time=%dms\n",
                        args_array[i].courier_id,
                        args_array[i].completed_count,
                        args_array[i].total_time);
    }

    fclose(fptr);

    printf("[CARGOGTU] SHIFT_END completed=%d cancelled=%d total_time=%dms\n", (int)completed_orders, (int)cancelled_orders, (int)total_delivery_time);

    printf("[CARGOGTU] SHUTDOWN_COMPLETE\n");
}