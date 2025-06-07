#include "vmixer.h"
#include "string.h"
#include "vhash.h"

static void vmixer_index_create_vertex_set(IndexAttributeList *,
                                           VertexAttributeList **,
                                           VertexHashTable *, VertexBuffer *,
                                           IndexBuffer *);

/**
   Combine and store each unique vertex list into a set.
   Use the hash insert return value to determine if the vertex array shall
  be added to the destination.

   Index (1 / 3 / 0)
      '- position = 1
      '- uv = 3
      '- normal = 0

   Vertex:
      '- position   0.0f 1.0f 0.0f 2.0f 2.0f 4.0f 1.0f 0.0f 0.3f...
                                 '-----[1]------'

      '- UV:        0.3f 0.2f 1.0f 0.4f 0.3f 4.0f 1.0f 0.4f 0.2f...
                                                 '---[3]---'

      '- normal:    0.1f 1.0f 0.4f 0.1f 0.3f 0.2f 1.0f 0.4f 1.3f...
                   '-----[0]-----'


   Set:
   .--------------------------------------------------------------------------.
   |  Key |                            Value                            | Id  |
   |------+-------------------------------------------------------------+-----|
   |      |  2.0f 2.0f 4.0f  0.1f 1.0f 0.4f  0.0f 0.0f 0.0f  1.0f 0.4f  |     |
   | hash | '------.-------''-------.------''-------.------''----.----' | len |
   |      |     position          normal          color         UV      |     |
   '------'-------------------------------------------------------------'-----'

   Those unique vertex composition will then be narrowed and clamped between
  the max(index_count) as to obtain an linear list. To easily traverse the
  occupied index, the hash set has a occupied list that direnctly points to
  the occupied entries.

   On top of storing the vertex attribtues, the Id (index) is stored as well
  and corressponds to the hash table length at the moment T when the value
  is added. This ensures a unique id for each entries and will make it
   easier to directly create the index list (0 , 3, 7, 3, 2, 3) by mapping
  from the hash:

                  .------  Values
   hash(values) -<            8
                  '------  Index
 */
void vmixer_index_create_vertex_set(IndexAttributeList *index_list,
                                    VertexAttributeList **attr_list,
                                    VertexHashTable *table, VertexBuffer *vb,
                                    IndexBuffer *ib) {

  // Store unique index combination in hash list along with their vertex
  for (size_t i = 0; i < index_list->length; i++) {

    IndexAttributeGroup *group = &index_list->entries[i];

    // attributes
    for (size_t g = 0; g < group->length; g++) {

      IndexAttribute *attributes = &group->entries[g];

      // create the vertex list based on the index attributes
      mbin_vertex_t vertices[VERTEX_STRIDE];
      size_t offset = 0;

      // define position
      size_t p = attributes->position;
      VertexAttributeList *p_list = attr_list[0];
      memcpy(vertices + offset, &p_list->entries[p * p_list->dimension],
             p_list->dimension * sizeof(mbin_vertex_t));
      offset += p_list->dimension;

      // define normal
      size_t n = attributes->normal;
      VertexAttributeList *n_list = attr_list[1];
      memcpy(vertices + offset, &n_list->entries[n * n_list->dimension],
             n_list->dimension * sizeof(mbin_vertex_t));
      offset += n_list->dimension;

      // define color
      memcpy(vertices + offset, (mbin_vertex_t[3]){0.0f, 0.0f, 0.0f},
             3 * sizeof(mbin_vertex_t));
      offset += 3;

      // define uv
      size_t u = attributes->uv;
      VertexAttributeList *u_list = attr_list[2];
      memcpy(vertices + offset, &u_list->entries[u * u_list->dimension],
             u_list->dimension * sizeof(mbin_vertex_t));

      mbin_index_t index;
      int insert_result = vhash_insert(table, vertices, &index);

      if (insert_result == VHASH_SUCCESS) {
        // if inserted, push values in the buffers
        vertex_buffer_insert(vb, vertices, VERTEX_STRIDE);
        index_buffer_insert(ib, index);
      } else if (insert_result == VHASH_EXIST) {
        // else if already exist and could find it, add the index
        VertexHashKey *search_result = vhash_search(table, vertices);
        if (search_result)
          index_buffer_insert(ib, search_result->index);
      }
    }
  }
}

/**
   Read each index group from the list,
 */
int vmixer_index_compose_from_vertex(IndexAttributeList *index_list,
                                               VertexAttributeList **attr_list,
                                               VertexBuffer *vb,
                                               IndexBuffer *ib) {

  VertexHashTable table;
  if (vhash_create(&table, VHASH_BASE_CAPACITY) == 0) {
    vmixer_index_create_vertex_set(index_list, attr_list, &table, vb, ib);
  } else {
    perror("Couldn't create hash table for index composing\n");
    return VINDEX_ALLOC_FAILURE;
  }

  return VINDEX_SUCCESS;
}

/**
   Generate line direction norm(A-B) from a given vertex list
 */
void vmixer_attribute_line_direction(const VertexAttributeList *src,
                                     const IndexAttributeList *index,
                                     VertexAttributeList *dest) {

    
}
