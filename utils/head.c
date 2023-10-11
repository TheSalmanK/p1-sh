#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINELEN 1024

static void usage(void);

int
main(int argc, char *argv[]) {
    FILE *file = stdin;
    char line[LINELEN];
    int num_lines = 5; // Default number of lines
    int current_line = 0;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0) {
            if (i + 1 < argc) {
                num_lines = atoi(argv[++i]);
            } else {
                usage();
                return EXIT_FAILURE;
            }
        } else {
            file = fopen(argv[i], "r");
            if (!file) {
                perror("head");
                return EXIT_FAILURE;
            }
        }
    }

    // Print the first num_lines lines
    while (current_line < num_lines && fgets(line, LINELEN, file)) {
        printf("%s", line);
        current_line++;
    }

    if (file != stdin) {
        fclose(file);
    }

    return EXIT_SUCCESS;
}

static void usage(void) {
    printf("head, prints the first few lines of a file\n");
    printf("usage: head [FLAG] FILE\n");
    printf("FLAG can be:\n");
    printf("  -n N     show the first N lines (default 5)\n");
    printf("If no FILE specified, read from STDIN\n");
}
