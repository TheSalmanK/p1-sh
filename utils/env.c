#include <stdio.h>
#include <stdlib.h>

static void usage (void);

int
main (int argc, char *argv[], char *envp[])
{
  return EXIT_SUCCESS;
}

static void
usage (void)
{
  printf ("env, set environment variables and execute program\n");
  printf ("usage: env [name=value ...] PROG ARGS\n");
}

