#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Given a message as input, print it to the screen followed by a
// newline ('\n'). If the message contains the two-byte escape sequence
// "\\n", print a newline '\n' instead. No other escape sequence is
// allowed. If the sequence contains a '$', it must be an environment
// variable or the return code variable ("$?"). Environment variable
// names must be wrapped in curly braces (e.g., ${PATH}).
//
// Returns 0 for success, 1 for errors (invalid escape sequence or no
// curly braces around environment variables).
extern int last_return_code;

int
echo (char *message)
{
  char *start = message;
  char prev_char = 0;

  while (*start)
    {
      if (strncmp (start, "\\n", 2) == 0)
        {
          putchar ('\n');
          start += 2;
          prev_char = '\n';
        }
      else if (strcmp (start, "$?") == 0)
        {
          printf ("%d", last_return_code);
          start += 2;
          prev_char = *(start - 1);
        }
      else if (*start == '$' && *(start + 1) == '{')
        {
          char *end = strchr (start, '}');
          if (!end)
            {
              return 1; // Error: unmatched curly brace
            }
          char varname[100];
          strncpy (varname, start + 2, end - start - 2);
          varname[end - start - 2] = '\0';

          // Directly retrieve the value associated with the key
          char *value = hash_find (varname);
          if (value)
            {
              printf ("%s", value);
            }
          start = end + 1;
          prev_char = *(start - 1);
        }
      else
        {
          // If the current character and previous character are spaces, skip
          // this iteration
          if (!(*start == ' ' && prev_char == ' '))
            {
              putchar (*start);
            }
          prev_char = *start;
          start++;
        }
    }
  putchar ('\n');
  return 0;
}

// Given a key-value pair string (e.g., "alpha=beta"), insert the mapping
// into the global hash table (hash_insert ("alpha", "beta")).
//
// Returns 0 on success, 1 for an invalid pair string (kvpair is NULL or
// there is no '=' in the string).
int export(char *kvpair)
{

  if (!kvpair)
    {
      return 1;
    }
  char *equal_sign = strchr (kvpair, '=');
  if (!equal_sign)
    {
      return 1;
    }
  char key[100];
  strncpy (key, kvpair, equal_sign - kvpair);
  key[equal_sign - kvpair] = '\0';
  hash_insert (key, equal_sign + 1);

  // Debugging: Check if the value was correctly inserted
  char *debug_value = hash_find (key);

  return 0;
}

// Prints the current working directory (see getcwd()). Returns 0.
int
pwd (void)
{
  char cwd[1024];
  if (getcwd (cwd, sizeof (cwd)) != NULL)
    {
      printf ("%s\n", cwd);
    }
  else
    {
      perror ("pwd error");
      return 1;
    }
  return 0;
}

// Removes a key-value pair from the global hash table.
// Returns 0 on success, 1 if the key does not exist.
int
unset (char *key)
{
  return hash_remove (key) ? 0 : 1;
}

// Given a string of commands, find their location(s) in the $PATH global
// variable. If the string begins with "-a", print all locations, not just
// the first one.
//
// Returns 0 if at least one location is found, 1 if no commands were
// passed or no locations found.
int
which (char *cmdline)
{
  if (!cmdline)
    {
      return 1;
    }

  // Check for built-in commands
  if (strcmp (cmdline, "cd") == 0 || strcmp (cmdline, "echo") == 0
      || strcmp (cmdline, "pwd") == 0 || strcmp (cmdline, "which") == 0
      || strcmp (cmdline, "export") == 0 || strcmp (cmdline, "unset") == 0)
    {
      printf ("%s: dukesh built-in command\n", cmdline);
      return 0;
    }

  // Check for relative paths
  if (strncmp (cmdline, "./", 2) == 0)
    {
      if (access (cmdline, F_OK) == 0)
        {
          printf ("%s\n", cmdline);
          return 0;
        }
    }

  int all = 0;
  if (strncmp (cmdline, "-a", 2) == 0)
    {
      all = 1;
      cmdline += 3; // skip "-a "
    }
  char *path = getenv ("PATH");
  char *token = strtok (path, ":");
  int found = 0;
  while (token)
    {
      char fullpath[1024];
      snprintf (fullpath, sizeof (fullpath), "%s/%s", token, cmdline);
      if (access (fullpath, F_OK) == 0)
        {
          printf ("%s\n", fullpath);
          found = 1;
          if (!all)
            {
              break;
            }
        }
      token = strtok (NULL, ":");
    }
  return found ? 0 : 1;
}
