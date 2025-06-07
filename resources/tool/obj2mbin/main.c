#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/buffer.h"
#include "lib/file.h"
#include "lib/mbin.h"
#include "lib/vattr.h"
#include "lib/vindex.h"
#include "lib/vmixer.h"

/**
   For faces:
   1. cache attributes
   2. create triangles
   3. compose
 */
void cache_faces(IndexAttributeList *cached_faces_index,
                 VertexAttributeList **cached_attributes, VertexBuffer *vb,
                 IndexBuffer *ib, FILE *file) {

  index_attribute_cache(file, cached_faces_index, "f ", "%d/%d/%d");

  // trianglify face index list
  index_attribute_triangulate(cached_faces_index);

  // compose faces
  vmixer_index_compose_from_vertex(cached_faces_index, cached_attributes, vb,
                                   ib);

#ifdef VERBOSE
  index_attribute_print(cached_faces_index);
#endif
}

/**
   For lines, the idea is to manipulate a copy of the initial vertex list as to
   make it fit the face method, by:
   1. copying the positions into the normals
   2. create a mock uv  { sides, thickness, -side, thickness }
   3. for each index group set the normal as the opposite position
   4. duplicate each group attributes (doublon) and assign uv to 1 so it has
   opposite sides

   By doing so we can simply reuse the same trigangulatio and composition
   functions (initially used for the faces)
 */
void cache_lines(IndexAttributeList *cached_lines_index,
                 VertexAttributeList **cached_attributes, VertexBuffer *vb,
                 IndexBuffer *ib, FILE *file) {

  index_attribute_cache(file, cached_lines_index, "l ", "%d");

  if (cached_lines_index->length) {

    // copy vertex  attribute to line normal
    VertexAttributeList cached_line_normal;
    VertexAttributeList *cached_position =
        cached_attributes[VertexAttributeListIndex_Position];

    vertex_attribute_copy(cached_position, &cached_line_normal);

    // create uv attributes
    const size_t new_uv_length = 8;
    VertexAttributeList cached_line_uv = {
        .capacity = new_uv_length,
        .length = 0,
        .entries = NULL,
        .dimension = 2,
    };

    const float thickness = 0.005f;
    const float A_mul = 1.0f;
    const float B_mul = -1.0f;
    mbin_vertex_t new_uv[new_uv_length] = {

        // A +1
        1.0f,  // side
        A_mul, // direction mul
	
        // A -1
        -1.0f, // side
        A_mul, // direction mul
	
        // B +1
        1.0f,  // side
        B_mul, // direction mul

        // B -1
        -1.0f, // side
        B_mul, // direction mul

    };

    vertex_attribute_list_insert(&cached_line_uv, new_uv, new_uv_length);

    // set line opposite vertex
    index_attribute_line_set_opposite(cached_lines_index);

    // set doublon
    index_attribute_line_set_doublon(cached_lines_index);

    // compose
    // create a dedicated new vertex attribute list for the lines with the
    // replaced "normals" as well a mock uv
    VertexAttributeList *cached_lines_attributes[3] = {
        cached_attributes[VertexAttributeListIndex_Position],
        &cached_line_normal, // replace normal list with new one (position)
        &cached_line_uv,
    };

    // trianglify face index list
    index_attribute_triangulate(cached_lines_index);

#ifdef VERBOSE
    index_attribute_print(cached_lines_index);
#endif

    vmixer_index_compose_from_vertex(cached_lines_index,
                                     cached_lines_attributes, vb, ib);
  }
}

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
    return FILE_OPEN_FAIL;
  }

  VertexAttributeList cached_position = {
      .entries = NULL,
      .capacity = VERTEX_LIST_CAPACITY,
      .prefix = VERTEX_POSITION_LINE_PREFIX,
      .dimension = 3,
  };

  VertexAttributeList cached_normal = {
      .entries = NULL,
      .capacity = VERTEX_LIST_CAPACITY,
      .prefix = VERTEX_NORMAL_LINE_PREFIX,
      .dimension = 3,
  };

  VertexAttributeList cached_uv = {
      .entries = NULL,
      .capacity = VERTEX_LIST_CAPACITY,
      .prefix = VERTEX_UV_LINE_PREFIX,
      .dimension = 2,
  };

  // traverse obj file and cache vertex attributes
  VertexAttributeList *cached_attributes[3];
  cached_attributes[VertexAttributeListIndex_Position] = &cached_position;
  cached_attributes[VertexAttributeListIndex_Normal] = &cached_normal;
  cached_attributes[VertexAttributeListIndex_Uv] = &cached_uv;

  vertex_attribute_cache(f, cached_attributes);

#ifdef VERBOSE
  for (int v = 0; v < 3; v++)
    vertex_attribute_print(cached_attributes[v]);
#endif

  // cache faces index
  IndexAttributeList cached_faces_index = {
      .entries = NULL,
      .capacity = 0,
      .length = 0,
  };

  cache_faces(&cached_faces_index, cached_attributes, vb, ib, f);

  // case line index
  IndexAttributeList cached_lines_index = {
      .entries = NULL,
      .capacity = 0,
      .length = 0,
  };

  cache_lines(&cached_lines_index, cached_attributes, vb, ib, f);

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
    char in_filename[256], out_file[512];

    namefile_from_path(argv[i], in_filename, 256);
    fprintf(stdout, "%s.obj...\n", in_filename);

    // define vertex filename
    snprintf(out_file, sizeof(out_file), "%s/%s.mbin", out_dir, in_filename);

    const char *path = argv[i];

    VertexBuffer vb = {
        .capacity = MBIN_BUFFER_DEFAULT_CAPACITY,
        .length = 0,
        .entries = malloc(sizeof(mbin_vertex_t) * MBIN_BUFFER_DEFAULT_CAPACITY),
    };

    IndexBuffer ib = {
        .capacity = MBIN_BUFFER_DEFAULT_CAPACITY,
        .length = 0,
        .entries = malloc(sizeof(mbin_vertex_t) * MBIN_BUFFER_DEFAULT_CAPACITY),
    };

    // convert OBJ to Mesh Binary format
    convert_obj_to_mbin(path, out_dir, &vb, &ib);

    // build mbin
    MBINFile *mbin_file;

    mbin_create(&mbin_file, &(MBINFileCreateDescriptor){
                                .index_length = ib.length,
                                .vertex_length = vb.length,
                                .index_size_type = sizeof(mbin_index_t),
                                .vertex_size_type = sizeof(mbin_vertex_t),
                            });

    buffer_merge_data(&vb, &ib, mbin_file->data);
    mbin_write_buffer(out_file, mbin_file);

    vertex_buffer_free(&vb);
    index_buffer_free(&ib);
    mbin_free(&mbin_file);

    printf("done\n");
  }

  return 0;
}
