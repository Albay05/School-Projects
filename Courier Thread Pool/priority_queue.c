#include "priority_queue.h"
#include <stdlib.h>


int compare_orders(order_t* order1, order_t* order2) {
    if(order1 == NULL || order2 == NULL) {
        perror("Invalid parameter error");
        exit(EXIT_FAILURE);
    }

    if(order1->priority_level < order2->priority_level) { // Higher priority
        return 1;
    }
    else if(order1->priority_level > order2->priority_level) { //Lower priority
        return -1;
    }
    else {
        if(order1->id < order2->id) {
            return 1;
        }
        else if(order1->id > order2->id) {
            return -1;
        }
        else {
            return 0;
        }
    }
}

void insert2queue(PriorityQueue* queue, Node* node) {
    if(queue == NULL) {
        perror("Error accessing the queue");
        exit(EXIT_FAILURE);
    }

    Node* element = queue->head;
    Node* previous_element;

    if(queue->head == NULL || compare_orders(&node->order, &queue->head->order) > 0) {
        node->next = queue->head;
        queue->head = node;
        queue->size++;
    }
    else {
        while(element != NULL && compare_orders(&node->order, &element->order) <= 0) {
            previous_element = element;
            element = element->next;
        }

        node->next = element;
        previous_element->next = node;
        queue->size++;
    }
}

Node* pull_from_queue(PriorityQueue* queue) {
    if(queue->head == NULL)
        return NULL;

    Node* temp = queue->head;
    queue->head = temp->next;
    queue->size--;
    
    temp->next = NULL;
    return temp;
}