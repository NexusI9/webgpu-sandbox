#include "loader.mbin.h"
#include "../utils/system.h"
#include "string.h"
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

int loader_mbin_load(MBINFile **file, const char *path) {

  // directly map data into memory for unix environments
  // open: directly communicate with linux kernel
  // fopen: provide FILE, does not depend on OS kernel
  size_t size;

#ifdef __unix__
  *file = (MBINFile *)loader_mbin_mmap(path, &size);
#else
  *file = (MBINFile *)loader_mbin_read(path, &size);
#endif

  if (file == NULL) {
    VERBOSE_ERROR("Error while loading Mesh Binary file\n");
    return MBIN_LOADER_LOAD_ERROR;
  }

  return MBIN_LOADER_SUCCESS;
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

  return (MBINFile *)data;
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

  return (MBINFile *)data;
}

/**
   Load MBIN into a Primitive
   Basically a wrapper that automatically define the references primitive
 */
int loader_mbin_load_primitive(MBINLoadPrimitiveDescriptor *desc) {

  MBINFile *mbin;

  if (loader_mbin_load(&mbin, desc->path)) {
    VERBOSE_ERROR("Error while loading Mesh Binary file to Primitive\n");
    return MBIN_LOADER_LOAD_ERROR;
  }

  // map referenced primitive vertex attribuets
  VertexAttribute *vert_attr = &desc->primitive->vertex;
  vert_attr->capacity = mbin->vertex_length;
  vert_attr->length = mbin->vertex_length;
  vert_attr->entries = malloc(sizeof(vattr_t) * mbin->vertex_length);

  if (vert_attr->entries == NULL) {
    VERBOSE_ERROR("Couldn't allocate memory for vertex attribute\n");
    return MBIN_LOADER_ALLOC_FAIL;
  }

  MBIN_U32Float converter;

  for (size_t v = 0; v < mbin->vertex_length; v++) {
    converter.u = mbin->data[v];
    vert_attr->entries[v] = converter.f;
  }

  // map referenced primitive index attributes
  VertexIndex *index_attr = &desc->primitive->index;
  index_attr->capacity = mbin->index_length;
  index_attr->length = mbin->index_length;
  index_attr->entries = malloc(sizeof(vindex_t) * mbin->vertex_length);

  if (index_attr->entries == NULL) {
    VERBOSE_ERROR("Couldn't allocate memory for vertex attribute\n");
    return MBIN_LOADER_ALLOC_FAIL;
  }

  memcpy(index_attr->entries, mbin->data + mbin->vertex_length,
         sizeof(mbin_data_t) * mbin->index_length);

  return MBIN_LOADER_SUCCESS;
}
