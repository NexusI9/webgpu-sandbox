#include "loader.mbin.h"
#include "../utils/system.h"
#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>

#ifdef __unix__
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

#ifdef __unix__
static void *loader_mbin_mmap(const char *, size_t *);
#endif
static void *loader_mbin_read(const char *, size_t *);

int loader_mbin_load(MBINLoadDescriptor *desc) {

  // directly map data into memory for unix environments
  // open: directly communicate with linux kernel
  // fopen: provide FILE, does not depend on OS kernel
#ifdef __unix__
  desc->vertex_data = (MBINVertex *)loader_mbin_mmap(desc->vertex_path, NULL);
  desc->index_data = (MBINIndex *)loader_mbin_mmap(desc->index_path, NULL);
#else
  desc->vertex_data = (MBINVertex *)loader_mbin_read(desc->vertex_path, NULL);
  desc->index_data = (MBINIndex *)loader_mbin_read(desc->index_path, NULL);
#endif

  if (desc->vertex_data == NULL || desc->index_data == NULL) {
    VERBOSE_ERROR("Error while loading Mesh Binary file\n");
    return 1;
  }

  return 0;
}

#ifdef __unix__
/**
   Map file directly to memory (UNIX only)
 */
static void *loader_mbin_mmap(const char *path, size_t *size) {

  int fd = open(path, O_RDONLY);

  if (fd < 0) {
    VERBOSE_ERROR("Could not open file\n");
    return NULL;
  }

  struct stat st;
  if (fstat(fd, &st)) {
    VERBOSE_ERROR("Could not read file\n");
    close(fd);
    return NULL;
  }

  void *data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);

  if (data == MAP_FAILED) {
    VERBOSE_ERROR("Mapped failed\n");
    return NULL;
  }

  if (size)
    *size = st.st_size;
  return data;
}

#endif

/**
   Fallback for non-UNIX environments, simply read the file and allocate it in
   memory
 */
static void *loader_mbin_read(const char *path, size_t *size) {

  FILE *f = fopen(path, "rb");

  if (!f) {
    VERBOSE_ERROR("Could not load file\n");
    return NULL;
  }

  // get file size
  fseek(f, 0, SEEK_END);
  size_t file_size = ftell(f);
  rewind(f);

  // allocate memory
  void *data = malloc(file_size);
  if (!data) {
    VERBOSE_ERROR("Could not allocate memory\n");
    fclose(f);
    return NULL;
  }

  // read file to buffer
  size_t read = fread(data, 1, file_size, f);
  if (read != file_size) {
    VERBOSE_ERROR("Could not read the entire file\n");
    free(data);
    fclose(f);
    return NULL;
  }

  fclose(f);
  if (size)
    *size = file_size;

  return data;
}

/**
   Load MBIN into a Primitive
   Basically a wrapper that automatically define the references primitive
 */
int loader_mbin_load_primitive(MBINLoadPrimitiveDescriptor *desc) {

  MBINIndex temp_index;
  MBINVertex temp_vertex;

  if (loader_mbin_load(&(MBINLoadDescriptor){
          .index_data = &temp_index,
          .index_path = desc->index_path,
          .vertex_data = &temp_vertex,
          .vertex_path = desc->vertex_path,
      })) {
    VERBOSE_ERROR("Error while loading Mesh Binary file to Primitive\n");
    return 1;
  }
  
  // map referenced primitive vertex attribuets
  VertexAttribute *vert_attr = &desc->primitive->vertex;
  vert_attr->capacity = temp_vertex.length;
  vert_attr->length = temp_vertex.length;
  vert_attr->entries = temp_vertex.data;

  // map referenced primitive index attributes
  VertexIndex *index_attr = &desc->primitive->index;
  index_attr->capacity = temp_index.length;
  index_attr->length = temp_index.length;
  index_attr->entries = temp_index.data;

  return 0;
}
