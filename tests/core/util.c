#include "util.h"
#include <stdio.h>
#include <stdlib.h>


char *
read_file (const char *path)
{
  FILE *file = fopen (path, "rb");
  if (file == NULL)
    {
      fprintf (stderr, "Could not open file \" %s \".\n", path);
      exit (74);
    }

  fseek (file, 0L, SEEK_END);
  size_t file_size = ftell (file);
  rewind (file);

  char *buffer = (char *) malloc (file_size + 1);
  if (buffer == NULL)
    {
      fprintf (stderr, "Not enough memeory \" %s \".\n", path);
      exit (74);
    }

  size_t bytes_read = fread (buffer, sizeof (char), file_size, file);
  if (bytes_read < file_size)
    {
      fprintf (stderr, "Could not open file \" %s \".\n", path);
      exit (74);
    }

  buffer[bytes_read] = '\0';
  return buffer;
}
