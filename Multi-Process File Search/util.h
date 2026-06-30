#ifndef UTILS_H
#define UTILS_H
#include <signal.h>
#include <stdio.h>

extern volatile sig_atomic_t terminate_requested;
extern volatile sig_atomic_t current_worker_matches;

// Checks if the pattern and the filaname matches
int match_pattern(const char* pattern, char* filename);

// Partitioning the directory
char** get_subdirectories(const char *root, int *sub_count);
// Creating the full path name
char* create_full_path(const char* dir, const char* name);

// Searches the file recursively
int search_recursive(const char *path, const char *pattern, int min_size, int level, FILE *temp_fp, int *scanned_count);
int search_recursive_root_only(const char *path, const char *pattern, int min_size, int *scanned_count);

// Signal helpers
void setup_parent_signals();
void setup_worker_signals();

void handle_worker_sigterm(int sig);

// Output formatters
void print_tree_entry(const char* name, int level, long size, int pid);
void print_summary(int total_workers, int total_scanned, int total_matches, int* worker_pids, int* worker_matches,int is_partial);

void handle_sigint(int sig);
void handle_sigusr1(int sig);
void handle_sigchld(int sig);

#endif