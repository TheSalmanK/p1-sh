#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void usage(void);

int main(int argc, char *argv[], char *envp[]) {
    // If the user asks for help
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        usage();
        return EXIT_SUCCESS;
    }

    // If there are no arguments, print all environment variables
    if (argc == 1) {
        for (int i = 0; envp[i] != NULL; i++) {
            if (strlen(envp[i]) < 5) {
                printf("%s\n", envp[i]);
            }
        }
        return EXIT_SUCCESS;
    }

    // If there are arguments, set the environment variables
    int i = 1;
    while (argv[i] && strchr(argv[i], '=') != NULL) {
        char *name = strtok(argv[i], "=");
        char *value = strtok(NULL, "=");
        printf("%s=%s\n", name, value);
        setenv(name, value, 1);
        i++;
    }


    // If there's a command after the environment variables, execute it
    if (argv[i] != NULL) {
        execvp(argv[i], &argv[i]);
        perror("Error executing command with new environment");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

static void usage(void) {
    printf("env, set environment variables and execute program\n");
    printf("usage: env [name=value ...] [PROG ARGS]\n");
    printf("       env -h|--help\n");
}
