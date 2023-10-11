#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void usage (void);

int
main (int argc, char *argv[])
{
  // If no arguments are provided or if help is requested
    if (argc != 2 || (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        usage();
        return EXIT_SUCCESS;
    }
  return EXIT_SUCCESS;
}

static void
usage (void)
{
  printf ("rm, remove a file\n");
  printf ("usage: rm FILE\n");
}
