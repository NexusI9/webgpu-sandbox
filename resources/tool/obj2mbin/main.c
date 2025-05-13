#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
   Convert OBJ file to Mesh binary files (vertex + index).
   Using binary files helps for faster memory mapping/ embed.
 */

typedef float mbin_vertex_t;
typedef uint32_t mbin_index_t;

typedef struct {
  mbin_vertex_t *data;
  size_t count;
} VertexBuffer;

typedef struct {
  mbin_index_t *data;
  size_t count;
} IndexBuffer;

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

void namefile_from_path(const char *path, char *dest, size_t max_length) {

  const char *base = strrchr(path, '/');
  base = base ? base + 1 : path;

  // check if name contain "." (.obj)
  const char *dot = strchr(base, '.');

  // get length on filename before the dot
  size_t length = dot ? (size_t)(dot - base) : strlen(base);

  // trim name
  if (length > max_length)
    length = max_length - 1;

  strncpy(dest, base, length);
  dest[length] = '\0';
}

int convert_obj_to_mbin(const char *in_path, const char *out_dir,
                        VertexBuffer *vb, IndexBuffer *ib) {

  FILE *f = fopen(in_path, "r");

  if (!f) {
    fprintf(stderr, "Failed to open file: %s\n", in_path);
    return 1;
  }

  char line[512];
  while (fgets(line, sizeof(line), f)) {

    // vertex line
    if (line[0] == 'v' && line[1] == ' ') {
      float x, y, z;
      sscanf(line, "v %f %f %f", &x, &y, &z);
      vb->data[vb->count++] = x;
      vb->data[vb->count++] = y;
      vb->data[vb->count++] = z;
    }
    // index line
    else if (line[0] == 'f') {
      uint32_t i1, i2, i3;
      sscanf(line, "f %u %u %u", &i1, &i2, &i3);
      ib->data[ib->count++] = i1;
      ib->data[ib->count++] = i2;
      ib->data[ib->count++] = i3;
    }
  }

  fclose(f);

  return 0;
}

int main(int argc, char **argv) {

  if (argc < 3) {
    fprintf(stderr, "Usage: %s <obj_path1> [<obj_path2> ...] <output_dir>\n",
            argv[0]);
    return 1;
  }

  const char *out_dir = argv[argc - 1];

  for (int i = 1; i < argc - 1; ++i) {
    char in_filename[256], vertex_out_file[512], index_out_file[512];

    namefile_from_path(argv[i], in_filename, 256);
    fprintf(stdout, "%s.obj... ", in_filename);

    // define vertex filename
    snprintf(vertex_out_file, sizeof(vertex_out_file), "%s/%s.vertex.mbin",
             out_dir, in_filename);

    // define index filename
    snprintf(index_out_file, sizeof(index_out_file), "%s/%s.index.mbin",
             out_dir, in_filename);

    const char *path = argv[i];
    VertexBuffer vb = {malloc(sizeof(mbin_vertex_t) * 1024), 0};
    IndexBuffer ib = {malloc(sizeof(mbin_index_t) * 1024), 0};

    convert_obj_to_mbin(path, out_dir, &vb, &ib);
    write_buffer(vertex_out_file, vb.data, vb.count, sizeof(mbin_vertex_t));
    write_buffer(index_out_file, ib.data, ib.count, sizeof(mbin_index_t));

    free(vb.data);
    free(ib.data);
    fprintf(stdout, "done\n");
  }

  return 0;
}
