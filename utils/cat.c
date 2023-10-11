#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage(void);

int main(int argc, char *argv[]) {
    // If no arguments are provided or if help is requested
    if (argc != 2 || (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        usage();
        return EXIT_SUCCESS;
    }

    // Open the file
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Read and print the contents of the file
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }

    // Close the file
    fclose(file);

    return EXIT_SUCCESS;
}

static void usage(void) {
    printf("cat, print the contents of a file\n");
    printf("usage: cat FILE\n");
    printf("       cat -h|--help\n");
}
