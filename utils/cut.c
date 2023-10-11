#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINELEN 1024

static void usage(void);

int main(int argc, char *argv[]) {
    FILE *file = stdin; // Default to standard input
    char line[LINELEN];
    char delimiter = ' '; // Default delimiter
    int field_num = 1; // Default field number

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            if (i + 1 < argc) {
                delimiter = argv[++i][0];
            } else {
                usage();
                return EXIT_FAILURE;
            }
        } else if (strcmp(argv[i], "-f") == 0) {
            if (i + 1 < argc) {
                field_num = atoi(argv[++i]);
                if (field_num <= 0) {
                    usage();
                    return EXIT_FAILURE;
                }
            } else {
                usage();
                return EXIT_FAILURE;
            }
        } else {
            file = fopen(argv[i], "r");
            if (!file) {
                perror("cut");
                return EXIT_FAILURE;
            }
        }
    }

    // Tokenize and print the specified field
    while (fgets(line, LINELEN, file)) {
    // Remove newline character if present
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }

    char *token = strtok(line, &delimiter);
    int current_field = 1;
    int field_found = 0; // Flag to check if the desired field was found
    while (token) {
        if (current_field == field_num) {
            printf("%s\n", token);
            field_found = 1; // Set the flag to indicate the field was found
            break;
        }
        token = strtok(NULL, &delimiter);
        current_field++;
    }
    if (!field_found) { // If the desired field wasn't found, print a newline
        printf("\n");
    }
}

    if (file != stdin) {
        fclose(file);
    }

    return EXIT_SUCCESS;
}

static void usage(void) {
    printf("cut, splits each line based on a delimiter\n");
    printf("usage: cut [FLAG] FILE\n");
    printf("FLAG can be:\n");
    printf("  -d C     split each line based on the character C (default ' ')\n");
    printf("  -f N     print the Nth field (1 is first, default 1)\n");
    printf("If no FILE specified, read from STDIN\n");
}
