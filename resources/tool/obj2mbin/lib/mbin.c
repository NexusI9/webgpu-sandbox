#include "mbin.h"
#include <stdio.h>

int write_buffer(const char *path, void *data, size_t count, size_t type_size) {

  FILE *f = fopen(path, "wb");

  if (!f) {
    fprintf(stderr, "Failed to open file: %s\n", path);
    return 1;
  }

  fwrite(&type_size, sizeof(uint32_t), 1, f);
  fwrite(&count, sizeof(uint32_t), 1, f);
  fwrite(data, type_size, count, f);

  fclose(f);

  return 0;
}
