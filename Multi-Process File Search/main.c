#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include "util.h"

volatile sig_atomic_t terminate_requested = 0;
volatile sig_atomic_t finished_workers = 0;
volatile sig_atomic_t current_worker_matches = 0;
int num_of_workers = 0;
pid_t child_pids[8];
int child_matches[8];

char * USAGE_MESSAGE ="Usage: ./procSearch -d <root_dir> -n <num_workers> -f <pattern> [-s <min_size_bytes>]"
                        "\n\nOptions:"
                        "\n  -d  Root directory to search"
                        "\n  -n  Number of worker processes to fork (between 2 and 8, inclusive)"
                        "\n  -f  Filename pattern; supports the + operator"
                        "\n  -s  Match only files with size >= min_size bytes (Optional)\n";


void print_usage();
void handle_sigint(int sig) {
    (void)sig;
    terminate_requested = 1;
}
void handle_sigusr1(int sig) {
    (void)sig;
}
void handle_sigchld(int sig) {
    (void)sig;
    int status;
    pid_t pid;
    
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        finished_workers++;
        for (int i = 0; i < num_of_workers; i++) {
            if (child_pids[i] == pid) {
                if (WIFEXITED(status)) {
                    child_matches[i] = WEXITSTATUS(status);
                }
                else if (WIFSIGNALED(status) && WTERMSIG(status) != SIGTERM && WTERMSIG(status) != SIGKILL) {
                    char msg[128];

                    int len = snprintf(msg, sizeof(msg), "[Parent] Worker PID: %d terminated unexpectedly (exit status: %d).\n", pid, WTERMSIG(status));
                    write(STDERR_FILENO, msg, len);
                }
                break;
            }
        }
    }
}

int main(int argc, char* const argv[]) {
    if(argc == 1) {
        fprintf(stderr, "Error: No argument provided!\n");
        print_usage();
    }

    int opt;
    char* root_path = NULL;
    char* filename = NULL;
    int min_size = -1;

    while((opt = getopt(argc, argv, "d:n:f:s:")) != -1) {
        switch(opt) {
            case 'd':
                root_path = optarg; //optarg is the value following the flag
                break;
            case 'n':
                num_of_workers = atoi(optarg);
                break;
            case 'f':
                filename = optarg;
                break;
            case 's':
                min_size = atoi(optarg);
                break;
            default: // In case the flag is invalid
                fprintf(stderr, "Error: Invalid argument\n");
                print_usage();
                exit(EXIT_FAILURE);
        }
    }

    if(root_path == NULL || num_of_workers < 2 || num_of_workers > 8 || filename == NULL) { // No path parameter provided
        fprintf(stderr, "Insufficient-invalid parameters!\n");
        print_usage();
    }

    setup_parent_signals();

    int sub_count = 0;
    char** dir_array = get_subdirectories(root_path, &sub_count);
    int total_matches = 0;
    int total_scanned = 0;

    if(sub_count == 0) {
        printf("Notice: no subdirectories found; parent will search root directly.\n");
        num_of_workers = 0;
    }
    else if(sub_count < num_of_workers && num_of_workers > 0) {
        printf("Notice: only %d subdirectories found; using %d workers instead of %d.\n", sub_count, sub_count, num_of_workers);
        num_of_workers = sub_count;
    }

    for (int i = 0; i < num_of_workers; i++) {
        pid_t pid = fork();
        if (pid == 0) {         // Worker process
            setup_worker_signals();

            int local_scanned = 0;
            char filename_buf[32];
            sprintf(filename_buf, "worker_%d.txt", getpid());
            FILE *fptr = fopen(filename_buf, "w+");

            fprintf(fptr, "Scanned:00000000\n");
            
            int local_matches = 0;
            // Round-Robin: Each worker searches for (i, i + num_workers, ...) indexed directories
            for (int j = i; j < sub_count; j += num_of_workers) {
                char* sub_path = create_full_path(root_path, dir_array[j]);

                local_matches += search_recursive(sub_path, filename, min_size, 2, fptr, &local_scanned);
                free(sub_path);
            }

            rewind(fptr);
            fprintf(fptr, "Scanned:%08d", local_scanned);
            fclose(fptr);
            
            // Returns the number of matches after the work is done
            kill(getppid(), SIGUSR1);
            exit(local_matches % 256);
        }
        else {
            child_pids[i] = pid; // Parent saves the child's PID 
        }
    }

    int parent_matches = search_recursive_root_only(root_path, filename, min_size, &total_scanned);

    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 10000000; // 10 milisaniye

    while(finished_workers < num_of_workers && !terminate_requested) {
        nanosleep(&ts, NULL); 
    }

    if(terminate_requested) {
        printf("[Parent] SIGINT received. Terminating all workers...\n");
        for(int i = 0; i < num_of_workers; i++) {
            kill(child_pids[i], SIGTERM);
        }
        
        int time_left = 3;
        while(time_left > 0) {
            time_left = sleep(time_left);
        }

        for (int i = 0; i < num_of_workers; i++) {
            if (kill(child_pids[i], 0) == 0) {
                kill(child_pids[i], SIGKILL);
            }
        }
    }

    printf("\n%s\n", root_path);
    for(int i = 0; i < num_of_workers; i++) {
        char fname[32];
        sprintf(fname, "worker_%d.txt", child_pids[i]);

        FILE* f = fopen(fname, "r");
        if(f) {
            char line[512];

            if (fgets(line, sizeof(line), f)) {
                char *ptr = strrchr(line, ':'); 
                if (ptr) {
                    total_scanned += atoi(ptr + 1);
                }
            }

            int lv;
            long sz;
            char pth[256];
            while (fscanf(f, "%d %ld %[^\n]", &lv, &sz, pth) == 3) {
                // Sadece dosya ismini almak için strrchr kullanılabilir
                char *name = strrchr(pth, '/');
                char *final_name = name ? name + 1 : pth;

                if(sz == -1) {
                    printf("|");
                    for(int k = 0; k < lv; k++) printf("------");
                    printf(" %s\n", final_name);
                }

                else
                    print_tree_entry(final_name, lv, sz, child_pids[i]);
            }

            fclose(f);
            unlink(fname);
        }
    }

    for(int i = 0; i < num_of_workers; i++)
        total_matches += child_matches[i];

    total_matches += parent_matches;

    if(total_matches == 0) {
        printf("No matching files found.\n");
    }

    print_summary(num_of_workers, total_scanned, total_matches, child_pids, child_matches, terminate_requested);

    for(int i = 0; i < sub_count; i++) {
        free(dir_array[i]);
    }
    free(dir_array);

    return 0;
}


void print_usage() {
    fprintf(stderr, "%s", USAGE_MESSAGE);
    exit(EXIT_FAILURE);
}