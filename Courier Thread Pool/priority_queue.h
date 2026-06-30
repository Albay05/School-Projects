#pragma once
#include "defs.h"

void insert2queue(PriorityQueue* queue, Node* node);
int compare_orders(order_t* order1, order_t* order2);
Node* pull_from_queue(PriorityQueue* queue);