#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtins.h"
#include "hash.h"
#include "process.h"

// No command line can be more than 100 characters
#define MAXLENGTH 100

int last_return_code = 0;

void
shell (FILE *input)
{
  hash_init (100);
  hash_insert ("?", "0");
  char buffer[MAXLENGTH];
  while (1)
    {
      printf ("$ ");
      memset (buffer, 0, sizeof (buffer));
      if (fgets (buffer, MAXLENGTH, input) == NULL)
        break;

      if (input != stdin)
        printf ("%s", buffer);
      fflush (stdout);

      buffer[strcspn (buffer, "\n")] = 0;

      if (strcmp (buffer, "quit") == 0)
        {
          break;
        }
      else if (strncmp (buffer, "echo ", 5) == 0)
        {
          echo (buffer + 5);
        }
      else if (strcmp (buffer, "pwd") == 0)
        {
          pwd ();
        }
      else if (strncmp (buffer, "cd ", 3) == 0)
        {
          if (chdir (buffer + 3) != 0)
            {
              perror ("cd error");
            }
        }
      else if (strncmp (buffer, "which ", 6) == 0)
        {
          which (buffer + 6);
        }
      else if (strncmp (buffer, "export ", 7) == 0)
        {
          export(buffer + 7);
        }
      else
        {
          // Tokenize the input
          char *tokens[100];
          int i = 0;
          tokens[i] = strtok (buffer, " ");
          while (tokens[i] != NULL)
            {
              i++;
              tokens[i] = strtok (NULL, " ");
            }
          pid_t pid;

          if ((pid = fork ()) == 0)
            { // Child process
              // Execute the command
              dup2 (fileno (input), STDIN_FILENO);
              execute_command (tokens);
              exit (EXIT_FAILURE); // This line will only be reached if
                                   // execute_command fails
            }
          else if (pid < 0)
            { // Forking failed
              perror ("Error forking");
            }
          else
            { // Parent process
              int status;
              waitpid (pid, &status, 0); // Wait for child process to complete
              if (WIFEXITED (status))
                {
                  last_return_code = WEXITSTATUS (status);
                }
            }
        }
    }
  printf ("\n");
  hash_destroy ();
}
