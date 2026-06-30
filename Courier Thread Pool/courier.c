#define _XOPEN_SOURCE 700
#include "courier.h"
#include "priority_queue.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


void* courier_routine(void* arg) {
    CourierArgs* c_args = (CourierArgs*)arg;

    int self_id = c_args->courier_id;

    while(1) {
        pthread_mutex_lock(&c_args->sm->queue_mutex);
        
        while(c_args->shared_queue->head == NULL && !global_shutdown_flag) {
            pthread_mutex_lock(&c_args->sm->log_mutex);
            printf("[COURIER-%d] WAITING\n",self_id);
            pthread_mutex_unlock(&c_args->sm->log_mutex);
            
            pthread_cond_wait(&c_args->sm->queue_cond, &c_args->sm->queue_mutex);
        }

        if(global_shutdown_flag || c_args->shared_queue->size == 0) {
            pthread_mutex_unlock(&c_args->sm->queue_mutex);
            break;
        }

        Node* node = pull_from_queue(c_args->shared_queue);

        pthread_mutex_unlock(&c_args->sm->queue_mutex);

        pthread_mutex_lock(&c_args->sm->log_mutex);
        printf("[COURIER-%d] DELIVERY_START id=%d recipient=%s priority=%s\n",
                                            self_id,
                                            node->order.id,
                                            node->order.receiver_name,
                                            priority2string(node->order.priority_level));
        pthread_mutex_unlock(&c_args->sm->log_mutex);

        usleep(node->order.duration * 500 * 1000);

        completed_orders++;
        total_delivery_time += (node->order.duration * 500);

        c_args->completed_count++;
        c_args->total_time += (node->order.duration * 500);

        pthread_mutex_lock(&c_args->sm->log_mutex);
        printf("[COURIER-%d] DELIVERY_COMPLETE id=%d recipient=%s duration=%dms\n",
                                            self_id,                                
                                            node->order.id,
                                            node->order.receiver_name,
                                            node->order.duration);
        pthread_mutex_unlock(&c_args->sm->log_mutex);

        free(node);
    }

    pthread_mutex_lock(&c_args->sm->log_mutex);
    printf("[COURIER-%d] SHIFT_OVER\n",self_id);
    pthread_mutex_unlock(&c_args->sm->log_mutex);

    return NULL;
}

char* priority2string(priority_t pri) {
    switch(pri) {
        case 0:
            return "EXPRESS";
        case 1:
            return "STANDARD";
        case 2:
            return "ECONOMY";
        default:
            return NULL;
    }
}