#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "defs.h"
#include "courier.h"
#include "priority_queue.h"
#include "simulation.h"


int main(int argc, char *argv[]) {
    if(argc == 1) {
        print_usage();
    }

    int opt;
    int num_couriers = 0;
    char * input_file = NULL;
    char * output_file = NULL;

    while((opt = getopt(argc, argv, "n:i:s:")) != -1) {
        switch(opt) {
            case 'n':
                num_couriers = atoi(optarg);
                break;
            case 'i':
                input_file = optarg;
                break;
            case 's':
                output_file = optarg;
                break;
            default:
                print_usage();
        }
    }

    if(num_couriers < 1 || input_file == NULL || output_file == NULL) {
        fprintf(stderr, "Missing/invalid parameters.\n");
        print_usage();
    }

    run_simulation(num_couriers, input_file, output_file);
}

void print_usage() {
    fprintf(stderr, "%s", USAGE_MESSAGE);
    exit(EXIT_FAILURE);
}