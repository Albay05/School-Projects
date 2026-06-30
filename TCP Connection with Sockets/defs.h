#pragma once
#include <signal.h>
#include <time.h>

extern volatile sig_atomic_t stop_flag;

typedef struct ingredient_t {
    char name[17];
    int  quantity;
} ingredient_t;

typedef struct spellbook_t {
    ingredient_t items[64];
    int count;
} spellbook_t;

typedef struct client_t {
    int         fd;
    char        username[32];
    char        type[16];
    char        line_buf[512];
    time_t      last_active;
    spellbook_t spellbook;
} client_t;

int read_ing_file(char* filename, ingredient_t* array, int mode);