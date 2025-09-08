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
                 VertexAttributeList *cached_vertex_attributes,
                 VertexBuffer *vb, IndexBuffer *ib, FILE *file) {

  printf("=== CACHE FACES ===\n");
  index_attribute_cache(file, cached_faces_index, "f ", "%d/%d/%d");

  // trianglify face index list
  index_attribute_triangulate(cached_faces_index);

#ifdef VERBOSE
  if (cached_faces_index->length) {
    for (int v = 0; v < VERTEX_ATTRIBUTE_COUNT; v++)
      mbin_vertex_attribute_print(&cached_vertex_attributes[v]);

    index_attribute_print(cached_faces_index);
  }
#endif

  // compose faces
  vmixer_index_compose_from_vertex(cached_faces_index, cached_vertex_attributes,
                                   vb, ib);

  printf("> Faces done\n");
}

/**
   For lines, the idea is to manipulate a copy of the initial vertex list as to
   make it fit the face method, by:
   1. copying the positions into the normals
   2. create a mock uv  { sides, thickness, -side, thickness }
   3. for each index group set the normal as the opposite position
   4. duplicate each group attributes (doublon) and assign uv to 1 so it has
   opposite sides

   By doing so we can simply reuse the same trigangulation and composition
   functions (initially used for the faces)
 */
void cache_lines(IndexAttributeList *cached_lines_index,
                 VertexAttributeList *cached_attributes, VertexBuffer *vb,
                 IndexBuffer *ib, FILE *file, MBINIndexCacheMethod method) {

  printf("=== CACHE LINES ===\n");
  index_attribute_cache(file, cached_lines_index, "l ", "%d");

  if (cached_lines_index->length) {

    /* === Define Vertex Attributes ===
    Since lines use a different "vertex data structure" than faces we
    create a copy of the initial cahced list and populate the
    attributes accordingly:
       1. replace normal list with the opposite position
       2. replace the uv with side extrustion data
     */

    // position =>  normals
    mbin_vertex_attribute_copy(&cached_attributes[VertexAttributeType_Position],
                               &cached_attributes[VertexAttributeType_Normal],
                               VertexAttributeCopy_Shallow);

    // manually create uv attributes
    mbin_vertex_attribute_set_line_uv(
        &cached_attributes[VertexAttributeType_Uv]);

    /* === Define Index Attributes === */

    index_attribute_line_set_opposite(cached_lines_index);

    // set doublon
    if (method == MBINIndexCacheMethod_Wireframe)
      index_attribute_line_set_doublon(cached_lines_index);

    // trianglify face index list
    if (method == MBINIndexCacheMethod_Wireframe)
      index_attribute_triangulate(cached_lines_index);

#ifdef VERBOSE
    if (cached_lines_index->length) {
      for (int v = 0; v < VERTEX_ATTRIBUTE_COUNT; v++)
        mbin_vertex_attribute_print(&cached_attributes[v]);

      index_attribute_print(cached_lines_index);
    }
#endif

    /* === COMPOSE === */

    vmixer_index_compose_from_vertex(cached_lines_index,
                                     cached_attributes, vb, ib);
  }

  printf("> Lines done\n");
}

/**
   Convert OBJ file to Mesh binary files (vertex + index).
   Using binary files helps for faster memory mapping/ embedding as it
   directly match the respective struct data layout.
 */

int convert_obj_to_mbin(const char *in_path, const char *out_dir,
                        VertexBuffer *vb, IndexBuffer *ib) {

  FILE *f = fopen(in_path, "r");

  if (!f) {
    fprintf(stderr, "Failed to open file: %s\n", in_path);
    return FILE_OPEN_FAIL;
  }

  // traverse obj file and cache vertex attributes
  VertexAttributeList cached_attributes[VERTEX_ATTRIBUTE_COUNT] = {
      [VertexAttributeType_Position] =
          {
              .label = "position",
              .capacity = VERTEX_LIST_CAPACITY,
              .prefix = VERTEX_LINE_PREFIX_POSITION,
              .dimension = VertexAttributeDimension_Position,
              .offset = VertexAttributeOffset_Position,
          },
      [VertexAttributeType_Normal] =
          {
              .label = "normal",
              .capacity = VERTEX_LIST_CAPACITY,
              .prefix = VERTEX_LINE_PREFIX_NORMAL,
              .dimension = VertexAttributeDimension_Normal,
              .offset = VertexAttributeOffset_Normal,
          },
      [VertexAttributeType_Tangent] =
          {
              .label = "tangent",
              .capacity = VERTEX_LIST_CAPACITY,
              .prefix = VERTEX_LINE_PREFIX_UNDEFINED,
              .dimension = VertexAttributeDimension_Tangent,
              .offset = VertexAttributeOffset_Tangent,
          },
      [VertexAttributeType_Color] =
          {
              .label = "color",
              .capacity = VERTEX_LIST_CAPACITY,
              .prefix = VERTEX_LINE_PREFIX_UNDEFINED,
              .dimension = VertexAttributeDimension_Color,
              .offset = VertexAttributeOffset_Color,
          },
      [VertexAttributeType_Uv] =
          {
              .label = "uv",
              .capacity = VERTEX_LIST_CAPACITY,
              .prefix = VERTEX_LINE_PREFIX_UV,
              .dimension = VertexAttributeDimension_Uv,
              .offset = VertexAttributeOffset_Uv,
          },
  };

  // Cache attributes in their respective array depending on the list prefix
  for (int v = 0; v < VERTEX_ATTRIBUTE_COUNT; v++) {

    VertexAttributeList *list = &cached_attributes[v];

    // init dynamic lists (replace with Dyli)
    list->entries = malloc(sizeof(mbin_vertex_t) * list->capacity);

    if (list->prefix == VERTEX_LINE_PREFIX_UNDEFINED)
      goto set_fallback;

    file_read_line_prefix(f, list->prefix, mbin_vertex_attribute_from_line,
                          &(VertexAttributeCallbackDescriptor){.list = list});

    // populate with 0 as fallback if no pattern found
    if (list->length == 0) {
    set_fallback:
      memset(list->entries, 0.0f, list->dimension * sizeof(mbin_vertex_t));
      list->length = list->dimension;
    }
  }

  // cache faces index
  IndexAttributeList cached_faces_index = {
      .entries = NULL,
      .capacity = VINDEX_DEFAULT_CAPACITY,
      .length = 0,
  };

  cache_faces(&cached_faces_index, cached_attributes, vb, ib, f);

  // case line index
  IndexAttributeList cached_lines_index = {
      .entries = NULL,
      .capacity = VINDEX_DEFAULT_CAPACITY,
      .length = 0,
  };

  cache_lines(&cached_lines_index, cached_attributes, vb, ib, f,
              MBINIndexCacheMethod_Default);

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
    fprintf(stdout, "****************\n\n %s.obj...\n\n****************\n\n",
            in_filename);

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
