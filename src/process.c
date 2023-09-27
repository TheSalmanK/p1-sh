// The contents of this file are up to you, but they should be related to
// running separate processes. It is recommended that you have functions
// for:
//   - performing a $PATH lookup
//   - determining if a command is a built-in or executable
//   - running a single command in a second process
//   - running a pair of commands that are connected with a pipe

#include "process.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

// HELPER FUNCTIONS

// Check if the command is in the ./bin/ directory

int is_bin_command(const char *command) {
    return strncmp(command, "./bin/", 6) == 0;
}

// Helper function to check if a string represents a positive integer
int is_positive_integer(const char *str) {
    if (str[0] == '\0') return 0;  // Empty string is not a positive integer
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return 0;
        }
    }
    return 1;
}

// END HELPER FUNCTIONS

void execute_env_command(char **tokens) {
    int i = 0;
    char **new_env = NULL; // This will hold the new environment variables
    int new_env_count = 0; // Count of new environment variables

    // Check if there are no arguments
    if (tokens[1] == NULL) {
        extern char **environ;
        for (char **env = environ; *env != 0; env++) {
            printf("%s\n", *env);
        }
        return;
    }

    // If there are arguments, set the values
    while (tokens[i + 1] && strchr(tokens[i + 1], '=') != NULL) {
        new_env_count++;
        i++;
    }

    // Allocate memory for the new environment
    new_env = (char **)malloc((new_env_count + 1) * sizeof(char *));
    for (int j = 0; j < new_env_count; j++) {
        new_env[j] = tokens[j + 1];
    }
    new_env[new_env_count] = NULL;

    for (int j = 0; j < new_env_count; j++) {
        printf("%s\n", new_env[j]);
        fflush(stdout);
    }

    // Execute the program with the new environment
    if (tokens[i + 1]) {
        if (execve(tokens[i + 1], &tokens[i + 1], new_env) == -1) {
            perror("Error executing program with env");
            free(new_env);
            exit(EXIT_FAILURE);
        }
    }

    free(new_env);
}

// functionality for the cut command
void execute_cut_command(char **tokens) {
    char *args[100];
    int k = 0;  // Index for args array

    // Default command is "cut"
    args[k++] = "cut";

    // Flags for the cut command
    int found_d_flag = 0;
    int found_f_flag = 0;
    char *filename = NULL;  // To store the file name

    // Iterate through the tokens to find flags and file
    for (int j = 1; tokens[j] != NULL; j++) {
        if (strcmp(tokens[j], "-d") == 0) {
            found_d_flag = 1;
            args[k++] = "-d";
            if (tokens[j + 1] != NULL) {  // Ensure there's a value after -d
                args[k++] = tokens[++j];
            } else {
                printf ("cut, splits each line based on a delimiter\n");
                printf ("usage: cut [FLAG] FILE\n");
                printf ("FLAG can be:\n");
                printf ("  -d C     split each line based on the character C (default ' ')\n");
                printf ("  -f N     print the Nth field (1 is first, default 1)\n");
                printf ("If no FILE specified, read from STDIN\n");
                fflush(stdout);
                exit(EXIT_FAILURE);
            }
        } else if (strcmp(tokens[j], "-f") == 0) {
            found_f_flag = 1;
            args[k++] = "-f";
            if (tokens[j + 1] != NULL) {
                // Check if the value after -f is a positive integer
                int field_num = atoi(tokens[j + 1]);
                if (field_num <= 0) {
                    printf ("cut, splits each line based on a delimiter\n");
                    printf ("usage: cut [FLAG] FILE\n");
                    printf ("FLAG can be:\n");
                    printf ("  -d C     split each line based on the character C (default ' ')\n");
                    printf ("  -f N     print the Nth field (1 is first, default 1)\n");
                    printf ("If no FILE specified, read from STDIN\n");
                    fflush(stdout);
                    exit(EXIT_FAILURE);
                }
                args[k++] = tokens[++j];
            } else {
                printf ("cut, splits each line based on a delimiter\n");
                printf ("usage: cut [FLAG] FILE\n");
                printf ("FLAG can be:\n");
                printf ("  -d C     split each line based on the character C (default ' ')\n");
                printf ("  -f N     print the Nth field (1 is first, default 1)\n");
                printf ("If no FILE specified, read from STDIN\n");
                fflush(stdout);
                exit(EXIT_FAILURE);
            }
        } else {
            // Assume any other argument is a file
            filename = tokens[j];
        }
    }

    // If -d flag is not provided by the user, use the default value of space
    if (!found_d_flag) {
        args[k++] = "-d";
        args[k++] = " ";
    }

    // If -f flag is not provided by the user, use the default value of 1
    if (!found_f_flag) {
        args[k++] = "-f";
        args[k++] = "1";
    }

    // Add the file name to the end of the args array
    if (filename) {
        args[k++] = filename;
    }

    args[k] = NULL;  // Ensure args array is null-terminated

    // Execute the cut command with the arguments
    if (execvp("/bin/cut", args) == -1) {
        perror("Error executing cut command");
        exit(EXIT_FAILURE);
    }
}


// functionality for the head command
void execute_head_command(char **tokens) {
    char *args[100];
    int k = 0;  // Index for args array

    // Default command is "head"
    args[k++] = "head";

    // Flags for the head command
    int found_n_flag = 0;

    // Iterate through the tokens to find flags and file
    for (int j = 1; tokens[j] != NULL; j++) {
        if (strcmp(tokens[j], "-n") == 0) {
            found_n_flag = 1;
            args[k++] = "-n";
            if (tokens[j + 1] != NULL) {  // Ensure there's a value after -n
                args[k++] = tokens[++j];
            } else {
                fprintf(stderr, "head: missing argument to '-n'\n");
                exit(EXIT_FAILURE);
            }
        } else {
            // Assume any other argument is a file
            args[k++] = tokens[j];
        }
    }

    // If -n flag is not provided by the user, use the default value
    if (!found_n_flag) {
        args[k++] = "-n";
        args[k++] = "5";
    }

    args[k] = NULL;  // Ensure args array is null-terminated

    // Execute the head command with the arguments
    execvp("/bin/head", args);
}

// functionality for ls command
void execute_ls_command(char **tokens) {
    char *args[100];
    int k = 0;  // Index for args array

    // Default command is "ls"
    args[k++] = "ls";

    // Flags for the ls command
    int found_a_flag = 0;
    int found_s_flag = 0;

    // Iterate through the tokens to find flags and directory
    for (int j = 1; tokens[j] != NULL; j++) {
        if (strcmp(tokens[j], "-a") == 0) {
            found_a_flag = 1;
            args[k++] = "-a";  // Include '.' and '..'
        } else if (strcmp(tokens[j], "-s") == 0) {
            found_s_flag = 1;
            args[k++] = "-s";  // Print the allocated size of each file
        } else {
            // Assume any other argument is a directory
            args[k++] = tokens[j];
        }
    }

    // If no directory is specified, use the current directory
    if (!found_a_flag && !found_s_flag && tokens[1] == NULL) {
        args[k++] = ".";
    }

    args[k] = NULL;  // Ensure args array is null-terminated

    // Check if the file path is valid and exists
    if (access(args[k-1], F_OK) == -1) {
        fprintf(stderr, "ls: cannot access '%s': No such file or directory\n", args[k-1]);
        exit(EXIT_FAILURE);  // Exit with failure status
    }

    // Execute the ls command with the arguments
    execvp("/bin/ls", args);
}






// default call to run a utility
void execute_command(char **tokens) {
    char *command = tokens[0];
    if (command[0] == '.' && command[1] == '/') {
        command += 1;  // Skip the '.'
    }

    // invalid flag check
    for (int i = 1; tokens[i] != NULL; i++) {
        if (strncmp(tokens[i], "-l", 5) == 0 || strncmp(tokens[i], "-c", 5) == 0) {
            return;
        }
    }

    if (strcmp(command, "/bin/ls") == 0) {
        execute_ls_command(tokens);
        return;
    } else if (strcmp(command, "/bin/head") == 0) {
        execute_head_command(tokens);
        return;
    } else if (strcmp(command, "/bin/cut") == 0) {
        execute_cut_command(tokens);
        return;
    } else if (strcmp(command, "/bin/env") == 0) {
        execute_env_command(tokens);
        return;
    }

    if (access(command, X_OK) == 0) {
        if (execvp(command, tokens) == -1) {
            perror("Error executing command");
            exit(EXIT_FAILURE);
        }
    }
}



