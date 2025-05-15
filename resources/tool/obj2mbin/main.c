#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/mbin.h"
#include "lib/file.h"
#include "lib/vattr.h"
#include "lib/vindex.h"

#define VERTEX_LIST_CAPACITY 64
#define VERTEX_COLOR {0.0f, 0.0f, 0.0f}

/**
   Convert OBJ file to Mesh binary files (vertex + index).
   Using binary files helps for faster memory mapping/ embedding as it directly
   match the respective struct data layout.
 */

int convert_obj_to_mbin(const char *in_path, const char *out_dir,
                        VertexBuffer *vb, IndexBuffer *ib) {

  FILE *f = fopen(in_path, "r");

  if (!f) {
    fprintf(stderr, "Failed to open file: %s\n", in_path);
    return 1;
  }

  VertexAttributeList vertex_positions = {
      .capacity = VERTEX_LIST_CAPACITY,
      .dimension = 3,
      .prefix = "v ",
  };

  VertexAttributeList vertex_normals = {
      .capacity = VERTEX_LIST_CAPACITY,
      .dimension = 3,
      .prefix = "vn ",
  };

  VertexAttributeList vertex_uvs = {
      .capacity = VERTEX_LIST_CAPACITY,
      .dimension = 2,
      .prefix = "vt ",
  };

  VertexAttributeList *attributes[3] = {
      &vertex_positions,
      &vertex_normals,
      &vertex_uvs,
  };

  // cache attributes in their respective array
  for (int v = 0; v < 3; v++) {
    VertexAttributeList *list = attributes[v];
    file_read_line_prefix(f, list->prefix, vertex_attribute_from_line,
                          &(VertexAttributeCallbackDescriptor){.list = list});
#ifdef VERBOSE
    vertex_attribute_list_print(attributes[v]);
#endif
  }

  // read index
  // trianglify

  // build vertex atrtibute

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
    fprintf(stdout, "%s.obj...\n", in_filename);

    // define vertex filename
    snprintf(vertex_out_file, sizeof(vertex_out_file), "%s/%s.vertex.mbin",
             out_dir, in_filename);

    // define index filename
    snprintf(index_out_file, sizeof(index_out_file), "%s/%s.index.mbin",
             out_dir, in_filename);

    const char *path = argv[i];
    VertexBuffer vb = {0, malloc(sizeof(mbin_vertex_t) * 1024)};
    IndexBuffer ib = {0, malloc(sizeof(mbin_index_t) * 1024)};

    convert_obj_to_mbin(path, out_dir, &vb, &ib);
    // write_buffer(vertex_out_file, vb.data, vb.count, sizeof(mbin_vertex_t));
    // write_buffer(index_out_file, ib.data, ib.count, sizeof(mbin_index_t));

    free(vb.data);
    free(ib.data);

    printf("done\n");
  }

  return 0;
}
