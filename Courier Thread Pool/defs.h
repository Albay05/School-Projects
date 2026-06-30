#pragma once
#include <stdio.h>
#include <err.h>
#include <stdatomic.h>
#include <signal.h>
#include <bits/pthreadtypes.h>

extern volatile atomic_int completed_orders;
extern volatile atomic_int cancelled_orders;
extern volatile atomic_int total_delivery_time;
extern volatile sig_atomic_t global_shutdown_flag;

extern char* USAGE_MESSAGE;

void print_usage();

typedef enum priority_t {
    EXPRESS,
    STANDARD,
    ECONOMY
} priority_t;

typedef struct order_t {
    int         id;
    char        receiver_name[32];
    priority_t  priority_level;
    int         duration;
} order_t;

typedef struct Node {
    order_t order;
    struct Node*   next;
} Node;

typedef struct PriorityQueue {
    int     size;
    Node*   head;
} PriorityQueue;

typedef struct SimulationManager {
    int                     num_couriers;
    PriorityQueue*          queue;
    pthread_t*              couriers;
    pthread_mutex_t         queue_mutex;
    pthread_mutex_t         log_mutex;
    pthread_cond_t          queue_cond;
    volatile sig_atomic_t   shutdown_flag;
} SimulationManager;

typedef struct CourierArgs {
    int                 courier_id;
    PriorityQueue*      shared_queue;
    SimulationManager*  sm;
    int                 completed_count;
    int                 total_time;
} CourierArgs;