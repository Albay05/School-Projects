#define _DEFAULT_SOURCE
#include "util.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>


int match_pattern(const char* pattern, char* filename) {
    if(filename == NULL || pattern == NULL)
        return false;

    if(strlen(filename) == 1 && tolower(filename[0]) != tolower(pattern[0])) return false;

    int i = 0; // index of pattern
    int j = 0; // index of filename

    while(pattern[i] != '\0') {    // Keeps comparing until the end of pattern
        if(pattern[i] != '\0' && pattern[i+1] == '+') {
            char target = (char)tolower(pattern[i]);

            if(filename[j] == '\0' || tolower(filename[j]) != target) {
                return false;   // Return false if characters do not match
            }

            while(filename[j] != '\0' && tolower(filename[j]) == target) {
                j++;    // Consumes all the repeating letters
            }
            i += 2; // Skips the + character
        }
        else {
            if(filename[j] == '\0' || tolower(pattern[i]) != tolower(filename[j])) {
                return false;
            }
            i++;    // Increments both indexes as the characters match
            j++;
        }
    }
    return filename[j] == '\0';
}


void print_tree_entry(const char* name, int level, long size, int pid) {
    printf("|");
    for(int i = 0; i < level; i++) {
        printf("------");
    }
    int current_width = 40 - level * 6;

    printf(" %-*s", current_width,name);
    printf("(%ld bytes)\t[Worker %d]\n", size, pid);
    
}

void print_summary(int total_workers, int total_scanned, int total_matches, int* worker_pids, int* worker_matches,int is_partial) {
    if(is_partial) printf("-!-!- Partial Summary (Terminated by Signal) -!-!-\n");
    else printf("\n--- Summary ---\n");
    
    printf("%-20s: %d\n", "Total workers used", total_workers);
    printf("%-20s: %d\n", "Total files scanned", total_scanned);
    printf("%-20s: %d\n", "Total matches found", total_matches);
    for(int i = 0; i < total_workers; i++) {
        printf("Worker PID %-9d: %d match", worker_pids[i], worker_matches[i]);
        printf("%s", worker_matches[i] > 1 ? "es\n" : "\n");
    }
}

char* create_full_path(const char* dir, const char* name) {
    int len = strlen(dir) + strlen(name) + 2;
    char* full_path = malloc(len);

    if(!full_path) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    snprintf(full_path, len, "%s/%s", dir, name);
    return full_path;
}

void handle_worker_sigterm(int sig) {
    (void)sig;
    char msg[128];
    int len = snprintf(msg, sizeof(msg), "[Worker PID: %d] SIGTERM received. Partial matches: %d. Exiting.\n", getpid(), current_worker_matches);
    write(STDOUT_FILENO, msg, len);
    _exit(current_worker_matches % 256);
}

void setup_parent_signals() {
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);

    sa.sa_handler = handle_sigusr1;
    sigaction(SIGUSR1, &sa, NULL);

    sa.sa_handler = handle_sigchld;
    sigaction(SIGCHLD, &sa, NULL);
}
void setup_worker_signals() {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sa.sa_handler = SIG_IGN;
    sigaction(SIGINT, &sa, NULL);

    sa.sa_handler = handle_worker_sigterm; 
    sigaction(SIGTERM, &sa, NULL);
}


char** get_subdirectories(const char *root, int *sub_count) {
    char** dir_array = NULL;
    *sub_count = 0;

    DIR *dir;
    struct dirent *de;

    dir = opendir(root);        // Directory is opened
    if(dir == NULL) {
        perror("Error opening directory");
        return NULL;
    }

    while((de = readdir(dir)) != NULL) {
        if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
            continue;
        }

        if(de->d_type == DT_DIR) {
            char** temp = realloc(dir_array, sizeof(char*) * (*sub_count + 1));
            if(!temp) {
                perror("Error reallocating");
                return dir_array;
            }
            dir_array =temp;

            dir_array[*sub_count] = strdup(de->d_name);
            (*sub_count)++;
        }
    }

    if(closedir(dir) == -1) {   // Directory is closed and possible errors handled
        perror("Error closing directory");
        exit(EXIT_FAILURE);
    }

    return dir_array;
}



int search_recursive(const char *path, const char *pattern, int min_size, int level, FILE *temp_fp, int *scanned_count) {
    DIR *dir;
    struct dirent *de;

    int match_in_this_dir = 0;

    dir = opendir(path);        // Directory is opened
    if(dir == NULL) {
        perror("Error opening directory");
        return 0;
    }

    while((de = readdir(dir)) != NULL) {
        if(terminate_requested) {
            printf("[Worker PID: %d] SIGTERM received. Partial matches: %d. Exiting.\n", 
                getpid(), match_in_this_dir);
            break; 
        }

        if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
            continue;
        }

        int len = strlen(path) + strlen(de->d_name) + 2;
        char* full_path = malloc(len);

        if(full_path == NULL) {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }

        snprintf(full_path, len, "%s/%s", path, de->d_name);

        struct stat info;

        if(lstat(full_path, &info) == -1) { // File stats are taken from lstat call
            perror("Error taking file information");
            free(full_path);
            continue;
        }

        if(S_ISREG(info.st_mode)) {
            (*scanned_count)++;
            if(match_pattern(pattern, de->d_name) && (min_size == -1 || info.st_size >= min_size)) {
                
                // 1. ÖNCE KLASÖR İSMİ (Eğer bu klasörde ilk eşleşmeyse)
                if (match_in_this_dir == 0 && level > 0) {
                    fprintf(temp_fp, "%d -1 %s\n", level - 1, path); 
                }

                // 2. SONRA DOSYA BİLGİSİ
                printf("[Worker PID:%d] MATCH: %s (%ld bytes)\n", getpid(), full_path, info.st_size);
                fprintf(temp_fp, "%d %ld %s\n", level, (long)info.st_size, full_path);
                
                match_in_this_dir++;
                current_worker_matches++; // SIGTERM için sayaç [cite: 63, 64]
            }
        }
        else if(S_ISDIR(info.st_mode)) {
            match_in_this_dir += search_recursive(full_path, pattern, min_size, level+1, temp_fp, scanned_count);
        }
        
        free(full_path);
    }
    closedir(dir);
    return match_in_this_dir;
}

int search_recursive_root_only(const char *path, const char *pattern, int min_size, int *scanned_count) {
    DIR *dir = opendir(path);       // Directory is opened
    struct dirent *de;
    int matches = 0;

    if(dir == NULL) {
        perror("Error opening directory");
        return 0;
    }

    while((de = readdir(dir)) != NULL) {
        if(de->d_type == DT_REG) {
            (*scanned_count)++;
            struct stat info;

            int len = strlen(path) + strlen(de->d_name) + 2;
            char* full_path = malloc(len);

            if(full_path == NULL) {
                perror("Error allocating memory");
                exit(EXIT_FAILURE);
            }

            snprintf(full_path, len, "%s/%s", path, de->d_name);

            if(lstat(full_path, &info) == 0) {
                if(match_pattern(pattern, de->d_name) && (min_size == -1 || info.st_size >= min_size)) {
                    print_tree_entry(de->d_name, 0, info.st_size, getpid());
                    matches++;
                }
            }
            free(full_path);
        }
    }
    closedir(dir);
    return matches;
}