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

void loader_mbin_load(MBINLoadDescriptor *desc) {

  // directly map data into memory for unix environments
#ifdef __unix__
  desc->vertex_data = (MBINVertex *)loader_mbin_mmap(desc->vertex_path, NULL);
  desc->index_data = (MBINIndex *)loader_mbin_mmap(desc->index_path, NULL);
  return;
#endif

  desc->vertex_data = (MBINVertex *)loader_mbin_read(desc->vertex_path, NULL);
  desc->index_data = (MBINIndex *)loader_mbin_read(desc->index_path, NULL);
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
    VERBOSE_ERROR("Could not load file %s\n", path);
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
