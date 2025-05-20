#include "mbin.h"
#include <stdio.h>

int write_buffer(const char *path, MBINFile *mbin) {

  FILE *f = fopen(path, "wb");

  if (!f) {
    fprintf(stderr, "Failed to open file: %s\n", path);
    return 1;
  }

  // header
  fwrite(&mbin->header.vertex_length, sizeof(mbin_int), 1, f);
  fwrite(&mbin->header.vertex_size_type, sizeof(mbin_int), 1, f);

  fwrite(&mbin->header.index_length, sizeof(mbin_int), 1, f);
  fwrite(&mbin->header.index_size_type, sizeof(mbin_int), 1, f);

  // body data
  fwrite(&mbin->body.vertex, sizeof(mbin_vertex_t), mbin->header.vertex_length,
         f);
  fwrite(&mbin->body.index, sizeof(mbin_index_t), mbin->header.index_length, f);

  // fwrite(&count, sizeof(uint32_t), 1, f);
  // fwrite(data, type_size, count, f);

  fclose(f);

  return 0;
}
