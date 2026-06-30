#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <ctype.h>
#include <dirent.h>
#include <limits.h>

extern char *optarg;
extern int optind, opterr, optopt;

typedef struct Node {
    char name[PATH_MAX];        // File or directory name
    struct stat info;           // all the info taken from lstat
    int depth;                  // depth in the tree
    struct Node* firstChild;    // First file in the directory
    struct Node* nextSibling;   // Next file in the directory
} Node;

typedef struct SearchCriteria{
    char *target_path;
    char *filename_pattern;
    off_t size;
    char type;
    char *permissions;
    nlink_t links;          // Number of links
    int found_count;        // To keep track of the file if found
} SearchCriteria;

mode_t permission_masks[] = {
    S_IRUSR, S_IWUSR, S_IXUSR, // Owner
    S_IRGRP, S_IWGRP, S_IXGRP, // Group
    S_IROTH, S_IWOTH, S_IXOTH  // Others
};

// The error message to be displayed in case of an argument error
char * USAGE_MESSAGE ="Usage: ./myFind -w <directory> [-f <filename>] [-b <size>] [-t <type>] [-p <permissions>] [-l <links>]"
                        "\n\nOptions:"
                        "\n  -w  Target directory path to search recursively (Mandatory)"
                        "\n  -f  Filename with '+' support (case insensitive)"
                        "\n  -b  Exact file size in bytes"
                        "\n  -t  File type (d, s, b, c, f, p, l)"
                        "\n  -p  Permissions (e.g., 'rwxr-xr--')"
                        "\n  -l  Number of links\n";


Node* global_root = NULL; // Node pointer to hold the root data in case of a quit signal

bool check_filename(const char* filename, const char* arg);
bool match_criteria(struct stat *st, SearchCriteria *se, const char* filename);
Node* search_recursive(const char *path, int depth, SearchCriteria* criteria);
void print_tree(Node* root);
void free_tree(Node* root);
void print_usage();
void handle_sigint(int sig);


int main(int argc, char* const argv[]) {

    if(signal(SIGINT, handle_sigint) == SIG_ERR) {  // To handle stop signal
        fprintf(stderr, "Error: Could not initialize signal handler\n");
        exit(EXIT_FAILURE);
    }

    if(argc == 1) {
        fprintf(stderr, "Error: No argument provided!\n");
        print_usage();
    }

    int opt;
    char* path = NULL;
    char* filename = NULL;
    int size = -1;
    char type = 0;
    char* permissions = NULL;
    int links = -1;

    while((opt = getopt(argc, argv, "w:f:b:t:p:l:")) != -1) {
        switch(opt) {
            case 'w':
                path = optarg; //optarg is the value following the flag
                break;
            case 'f':
                filename = optarg;
                break;
            case 'b':
                size = atoi(optarg);
                break;
            case 't':
                type = optarg[0];
                break;
            case 'p':
                permissions = optarg;
                break;
            case 'l':
                links = atoi(optarg);
                break;
            default: // In case the flag is invalid
                fprintf(stderr, "Error: Invalid argument\n");
                print_usage();
        }
    }

    if(path == NULL) { // No path parameter provided
        fprintf(stderr, "-w parameter is mandatory!\n");
        print_usage();
    }

    if(filename == NULL && size == -1 && type == 0 && permissions == NULL && links == -1) {
        fprintf(stderr, "At least one of the search criteria must be provided!\n");
        print_usage();
    }

    if(permissions != NULL && strlen(permissions) != 9) {
        fprintf(stderr, "Error: Invalid permissions parameter, it should consist of 9 characters\n");
        print_usage();
    }

    SearchCriteria filters;     // Criteria assigned to the struct
    filters.target_path = path;
    filters.filename_pattern = filename;
    filters.size = size;
    filters.type = type;
    filters.permissions = permissions;
    filters.links = links;
    filters.found_count = 0;

    Node* root = search_recursive(path, 0, &filters);   // Search is done and node is assigned to the root
    global_root = root; // For the handler to able to access the root

    if(filters.found_count == 0) {
        printf("No file found!\n");
    }

    if(root != NULL) {
        print_tree(root);   // Displays the tree structure
        free_tree(root);    // Frees the whole tree
    }

    return 0;
}


Node* search_recursive(const char* current_path, int depth, SearchCriteria* criteria) {
    DIR *dir;
    struct dirent *de;

    Node *head = NULL, *tail = NULL;

    dir = opendir(current_path);        // Directory is opened
    if(dir == NULL) {
        perror("Error opening directory");
        return NULL;
    }

    while((de = readdir(dir)) != NULL) {    // Loop to read the directory with its sub directories
        if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
            continue;
        }

        char* full_path = malloc(strlen(current_path) + strlen(de->d_name) + 2);
        if(full_path == NULL) {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }

        snprintf(full_path, strlen(current_path) + strlen(de->d_name) + 2, "%s/%s", current_path, de->d_name);

        struct stat info;

        if(lstat(full_path, &info) == -1) { // File stats are taken from lstat call
            perror("Error taking file information");
            free(full_path);
            continue;
        }

        bool criteria_matches = match_criteria(&info, criteria, de->d_name);    // File is checked if all criteria matches

        if(criteria_matches)
            criteria->found_count++;    // File's found count incremented in case it is a match

        Node* sub_tree = NULL;
        if(S_ISDIR(info.st_mode)) {     // Recursively calls itself in case the file is a dir
            sub_tree = search_recursive(full_path, depth + 1, criteria);
        }

        if(criteria_matches || sub_tree != NULL) {
            Node* new_node = malloc(sizeof(Node));
            if(!new_node) {
                perror("Error allocating memory");
                exit(EXIT_FAILURE);
            }
            // Node's information is filled
            strncpy(new_node->name, de->d_name, PATH_MAX - 1);
            new_node->name[PATH_MAX - 1] = '\0';
            new_node->info = info;
            new_node->depth = depth;
            new_node->firstChild = sub_tree;
            new_node->nextSibling = NULL;

            if(head == NULL) {  // new_node is assigned to head if it is NULL
                head = new_node;
                if(depth == 0) {    // Root is assigned to the global root at first search
                    global_root = head;
                }
            }
            else
                tail->nextSibling = new_node;   // new_node is assigned to the sibling node if head is already searched

            tail = new_node;
        }
        free(full_path);
    }

    if(closedir(dir) == -1) {   // Directory is closed and possible errors handled
        perror("Error closing directory");
        exit(EXIT_FAILURE);
    }

    return head;
}


bool match_criteria(struct stat *st, SearchCriteria *se, const char* filename) {
    if(st == NULL || se == NULL || filename == NULL) {
        fprintf(stderr, "Error: Could not read stats for file: %s\n", filename);
        return false;
    }
    if(se->filename_pattern != NULL && !check_filename(se->filename_pattern, filename)) {
        return false;                               // file name control
    }
    if(se->size != -1 && st->st_size != se->size) { // file size control
        return false;
    }
    if(se->type != 0) {                             // file type control
        if(se->type == 'f' && !S_ISREG(st->st_mode)) return false;
        if(se->type == 'd' && !S_ISDIR(st->st_mode)) return false;
        if(se->type == 's' && !S_ISSOCK(st->st_mode)) return false;
        if(se->type == 'b' && !S_ISBLK(st->st_mode)) return false;
        if(se->type == 'c' && !S_ISCHR(st->st_mode)) return false;
        if(se->type == 'p' && !S_ISFIFO(st->st_mode)) return false;
        if(se->type == 'l' && !S_ISLNK(st->st_mode)) return false;
    }    
    if(se->permissions != NULL) {       // file permissions control
        for (int i = 0; i < 9; i++) {
            char expected = se->permissions[i];
            mode_t mask = permission_masks[i];

            if (expected != '-') {
                // If r, w or x is entered, that bit needs to be in st_mode
                if (!(st->st_mode & mask)) return false;
            } else {
                // If '-' is entered, that bit needs to NOT be in st_mode
                if (st->st_mode & mask) return false;
            }
        }
    }
    if((int)se->links != -1) {          // file link count control
        if((nlink_t)se->links != (nlink_t)st->st_nlink)
            return false;
    }

    return true;
}


bool check_filename(const char* filename, const char* arg) {
    if(arg == NULL || filename == NULL)
        return false;

    if(strlen(arg) == 1 && arg[0] != filename[0]) return false;

    int i = 0; // index of filename
    int j = 0; // index of arg

    while(filename[i] != '\0') {    // Keeps comparing until the end of filename
        if(filename[i] != '\0' && filename[i+1] == '+') {
            char target = (char)tolower(filename[i]);

            if(arg[j] == '\0' || tolower(arg[j]) != target) {
                return false;   // Return false if characters do not match
            }

            while(arg[j] != '\0' && tolower(arg[j]) == target) {
                j++;    // Consumes all the repeating letters
            }
            i += 2; // Skips the + character
        }
        else {
            if(arg[j] == '\0' || tolower(filename[i]) != tolower(arg[j])) {
                return false;
            }
            i++;    // Increments both indexes as the characters match
            j++;
        }
    }
    return arg[j] == '\0';
}


void print_tree(Node* root) {
    if(root == NULL) return;

    if(root->depth == 0) {  // Prints the main directory
        printf("%s\n", root->name);
    }
    else {
        printf("|");        // Prints the subdirectories wrt their depth
        int dash_count = (root->depth * 4) - 2;
        for(int i = 0; i < dash_count; i++) {
            printf("-");
        }
        printf("%s\n", root->name);
    }

    print_tree(root->firstChild);
    print_tree(root->nextSibling);
}


void free_tree(Node* root) {
    if(root == NULL) return;

    free_tree(root->firstChild);    // Firstly cleans the subdirectories recursively

    free_tree(root->nextSibling);   // Secondly cleans the sibling directories recursively

    free(root);                     // Finally cleans itself
}


void print_usage() {    // Prints usage info in case of a mistaken input
    fprintf(stderr, "%s", USAGE_MESSAGE);
    exit(EXIT_FAILURE);
}


void handle_sigint(int sig) {
    (void)sig;
    printf("\nTermination signal received. Cleaning up...\n");
    if(global_root != NULL) {
        free_tree(global_root); // Cleans the whole root after the signal is received
    }
    exit(0);
}