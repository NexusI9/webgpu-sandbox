#include "file.h"
#include "./system.h"
#include <stdio.h>
#include <stdlib.h>

#include "../utils/system.h"

void store_file(char **buffer, const char *path) {

  FILE *fp;
  long l_size;

  fp = fopen(path, "rb");

  if (!fp) {
    VERBOSE_ERROR("Couldn't load file.");
    exit(1);
  }

  fseek(fp, 0L, SEEK_END);
  l_size = ftell(fp);
  rewind(fp);

  /*allocate memory*/
  *buffer = malloc(l_size + 1);
  if (!*buffer)
    fclose(fp), fputs("Couldn't allocate memory", stderr), exit(1);

  /*copy file into buffer*/
  if (fread(*buffer, l_size, 1, fp) != 1)
    fclose(fp), fputs("Couldn't read entire file into memory", stderr), exit(1);

  /*Null terminate the buffer*/
  (*buffer)[l_size] = '\0';

  fclose(fp);
}
