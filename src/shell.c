#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "builtins.h"
#include "hash.h"
#include "process.h"

// No command line can be more than 100 characters
#define MAXLENGTH 100

int last_return_code = 0;

void execute_command_with_pipe(char **tokens) {
    int pipe_fd[2];
    pid_t child1, child2;

    // Find the position of the pipe in the tokens
    int pipe_pos = -1;
    for (int i = 0; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "|") == 0) {
            pipe_pos = i;
            break;
        }
    }

    if (pipe_pos == -1) {
        // No pipe found, execute the command normally
        execute_command(tokens);
        return;
    }

    // Split the tokens into two commands: before and after the pipe
    char *cmd1[pipe_pos + 1];
    char *cmd2[100 - pipe_pos];
    memcpy(cmd1, tokens, pipe_pos * sizeof(char *));
    memcpy(cmd2, &tokens[pipe_pos + 1], (100 - pipe_pos - 1) * sizeof(char *));
    cmd1[pipe_pos] = NULL;
    cmd2[100 - pipe_pos - 1] = NULL;

    // Create a pipe
    if (pipe(pipe_fd) == -1) {
        perror("Error creating pipe");
        exit(EXIT_FAILURE);
    }

    // First child process
    if ((child1 = fork()) == 0) {
        close(pipe_fd[0]);  // Close read end of the pipe
        dup2(pipe_fd[1], STDOUT_FILENO);  // Redirect STDOUT to write end of the pipe
        close(pipe_fd[1]);
        execvp(cmd1[0], cmd1);
        perror("Error executing first command");
        exit(EXIT_FAILURE);
    }

    // Second child process
    if ((child2 = fork()) == 0) {
        close(pipe_fd[1]);  // Close write end of the pipe
        dup2(pipe_fd[0], STDIN_FILENO);  // Redirect STDIN to read end of the pipe
        close(pipe_fd[0]);
        execvp(cmd2[0], cmd2);
        perror("Error executing second command");
        exit(EXIT_FAILURE);
    }

    close(pipe_fd[0]);
    close(pipe_fd[1]);
    waitpid(child1, NULL, 0);
    waitpid(child2, NULL, 0);
}

int contains_pipe(char **tokens) {
    for (int i = 0; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "|") == 0) {
            return 1;
        }
    }
    return 0;
}

void shell(FILE *input) {
    hash_init(100);
    hash_insert("?", "0");
    char buffer[MAXLENGTH];
    while (1) {
        printf("$ ");
        memset(buffer, 0, sizeof(buffer));
        if (fgets(buffer, MAXLENGTH, input) == NULL)
            break;

        if (input != stdin)
            printf("%s", buffer);
        fflush(stdout);

        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "quit") == 0) {
            break;
        } else if (strncmp(buffer, "echo ", 5) == 0) {
            echo(buffer + 5);
        } else if (strcmp(buffer, "pwd") == 0) {
            pwd();
        } else if (strncmp(buffer, "cd ", 3) == 0) {
            if (chdir(buffer + 3) != 0) {
                perror("cd error");
            }
        } else if (strncmp(buffer, "which ", 6) == 0) {
            which(buffer + 6);
        } else if (strncmp(buffer, "export ", 7) == 0) {
            export(buffer + 7);
        } else {
            // Tokenize the input
            char *tokens[100];
            int i = 0;
            tokens[i] = strtok(buffer, " ");
            while (tokens[i] != NULL) {
                i++;
                tokens[i] = strtok(NULL, " ");
            }

            pid_t pid;

            if ((pid = fork()) == 0) { // Child process
                // Check if the command contains a pipe
                if (contains_pipe(tokens)) {
                    execute_command_with_pipe(tokens);
                } else {
                    // Execute the command
                    execute_command(tokens);
                }
                exit(EXIT_FAILURE); // This line will only be reached if execute_command fails
            } else if (pid < 0) { // Forking failed
                perror("Error forking");
            } else { // Parent process
                int status;
                waitpid(pid, &status, 0); // Wait for child process to complete
                if (WIFEXITED(status)) {
                    last_return_code = WEXITSTATUS(status);
                }
            }
        }
    }
    printf("\n");
    hash_destroy();
}
