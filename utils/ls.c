#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

static void usage(void);
int compare(const void *a, const void *b);

int main(int argc, char *argv[]) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;
    char *path = "."; // Default directory
    int show_all = 0;
    int show_size = 0;
    char *entries[1000]; // Assuming max 1000 entries in a directory
    int count = 0;

    // If the user asks for help
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        usage();
        return EXIT_SUCCESS;
    }

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) {
            show_all = 1;
        } else if (strcmp(argv[i], "-s") == 0) {
            show_size = 1;
        } else if (strcmp(argv[i], "-sa") == 0) {
            show_all = 1;
            show_size = 1;
        } else {
            path = argv[i];
        }
    }

    dir = opendir(path);
    if (!dir) {
        return EXIT_FAILURE;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.' && !show_all) {
            continue;
        }
        entries[count++] = strdup(entry->d_name);
    }
    qsort(entries, count, sizeof(char *), compare);

    for (int i = 0; i < count; i++) {
    if (show_size) {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entries[i]);
        stat(full_path, &file_stat);
        
        if (S_ISDIR(file_stat.st_mode)) {
            // For directories, print the number of blocks
            printf("%ld ", file_stat.st_blksize);
        } else {
            // For regular files, print the size in bytes
            printf("%ld ", file_stat.st_size);
        }
    }
    printf("%s\n", entries[i]);
    free(entries[i]);
}


    closedir(dir);
    return EXIT_SUCCESS;
}

static void usage(void) {
    printf("ls, list directory contents\n");
    printf("usage: ls [FLAG ...] [DIR]\n");
    printf("FLAG is one or more of:\n");
    printf("  -a       list all files (even hidden ones)\n");
    printf("  -s       list file sizes\n");
    printf("If no DIR specified, list current directory contents\n");
}

int compare(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}
