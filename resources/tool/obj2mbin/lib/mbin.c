#include "mbin.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
   Allocate the nessecary storage for MBIN file on the heap since it uses as FAM
   (Flexible Array Member)

   If allocated on the stack (in which size if automatically asigned depending
   on type/struct members), the FMA isn't counted in it.
   In thi case size of MBIN is 16, which only include it 4 first uint32 members
   an NOT the last data array.

   As a result the data member isn't part of the alloacted memory block which
   lead inevitably to mem leak.

   As a result it's necessary to allocate the whole MBIN block with the data
   size included.
*/
int mbin_create(MBINFile **mbin, const MBINFileCreateDescriptor *desc) {

  size_t index_size = desc->index_length * desc->index_size_type;
  size_t vert_size = desc->vertex_length * desc->vertex_size_type;
  size_t data_size = vert_size + index_size;

  *mbin = (MBINFile *)malloc(sizeof(MBINFile) + data_size);

  if (mbin == NULL) {
    perror("Failed to allocate memory for MBIN object\n");
    return MBIN_ALLOC_FAIL;
  }

  (*mbin)->index_length = desc->index_length;
  (*mbin)->index_size_type = desc->index_size_type;
  (*mbin)->vertex_size_type = desc->vertex_size_type;
  (*mbin)->vertex_length = desc->vertex_length;

  return MBIN_SUCCESS;
}

int mbin_write_buffer(const char *path, MBINFile *mbin) {

  FILE *f = fopen(path, "wb");

  if (!f) {
    fprintf(stderr, "Failed to open file: %s\n", path);
    return 1;
  }

  size_t vert_size = mbin->vertex_length * sizeof(mbin_data_t);
  size_t index_size = mbin->index_length * sizeof(mbin_data_t);
  size_t data_size = vert_size + index_size;

#ifdef VERBOSE
  printf("\nMesh Binary:\n");
  mbin_print(mbin);
#endif

  fwrite(mbin, sizeof(MBINFile) + data_size, 1, f);
  fclose(f);

  return 0;
}

void mbin_print(MBINFile *mbin) {

  printf("Vertex Length:\t\t%d\n", mbin->vertex_length);
  printf("Vertex Size Type:\t%d\n", mbin->vertex_size_type);

  printf("Index Length:\t\t%d\n", mbin->index_length);
  printf("Index Size Type:\t%d\n", mbin->index_size_type);

  printf("Data:\n");

  /*
     Cannot cast (float) to print values as it simply converts it to the nearest
     float representation (1 => 1.0)

     So use type punning with: union members occupy same memory allocation.
     Can store data using 1 type and read it using another.
   */
  MBIN_U32Float converter;

  for (size_t v = 0; v < mbin->vertex_length; v++) {
    converter.u = mbin->data[v];
    printf("%f ", converter.f);
  }

  for (size_t i = 0; i < mbin->index_length; i++)
    printf("%u ", mbin->data[mbin->vertex_length + i]);

  printf("\n");
}

void mbin_free(MBINFile **mbin) {
  free(*mbin);
  *mbin = NULL;
}
