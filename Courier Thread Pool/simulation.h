#pragma once
#include "defs.h"

void read_file(char* filename, PriorityQueue *queue);
int parse_line(char line[], order_t* order);
void run_simulation(int num_couriers, char * input_file, char * output_file);