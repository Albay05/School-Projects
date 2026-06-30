#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"

volatile sig_atomic_t stop_flag = 0;

int read_ing_file(char* filename, ingredient_t* array, int mode) {
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
        char ing[17];
        int qua;
        if(sscanf(line, "%16s %d", ing, &qua) == 2) {
            if(qua > 0) {
                if(mode == 1) {
                    strcpy(array[valid_lines].name, ing);
                    array[valid_lines].quantity = qua;
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