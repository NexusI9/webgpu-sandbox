
#include "vindex.h"
#include "file.h"
#include "mbin.h"
#include "vattr.h"
#include "vhash.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/**
   Wavefront OBJ structure:

   vNosition:          vec3[]
   vNormal:            vec3[]
   vTextureCoordinate: vec2[]
   index:              Vp / vT / vN... []

   Need to convert to our vertex format:
   vertex attribute: position(3) normal(3) color(3) uv(2)...
   vertex index: f32[]

 */

static IndexAttributeGroup *index_attribute_new_group(IndexAttributeList *);
static IndexAttribute *index_attribute_new_attribute(IndexAttributeGroup *);
static int index_attribute_insert_group(char *, IndexAttributeGroup *);
static void index_attribute_from_line(const char *, void *);

void index_attribute_print(const IndexAttributeList *list) {
  printf("Index: \n");
  for (size_t g = 0; g < list->length; g++) {
    for (size_t i = 0; i < list->entries[g].length; i++) {
      IndexAttribute *attr = &list->entries[g].entries[i];
      printf("%d,%d,%d\t", attr->position, attr->normal, attr->uv);
    }
    printf("\n");
  }
}

int index_attribute_insert_group(char *line, IndexAttributeGroup *list) {

  // split values and push them into the current list
  // "1/3/4 1/9/4 3/2/1" => [ [1/3/4] , [1/9/4] , [3/2/1] ]
  char *index_group = strtok(line, INDEX_GROUP_SEPARATOR);

  while (index_group) {

    int iPosition, iUv, iNormal;
    sscanf(index_group, "%d/%d/%d", &iPosition, &iUv, &iNormal);

    // get new entry pointer
    IndexAttribute *new_attribute = index_attribute_new_attribute(list);
    if (new_attribute) {
      new_attribute->position = iPosition;
      new_attribute->normal = iNormal;
      new_attribute->uv = iUv;
    }

    index_group = strtok(0, INDEX_GROUP_SEPARATOR);
  }

  return 0;
}

IndexAttributeGroup *index_attribute_new_group(IndexAttributeList *list) {

  // check entries existence
  if (list->entries == NULL) {
    list->capacity = INDEX_DEFAULT_CAPACITY;
    list->length = 0;
    list->entries = malloc(sizeof(IndexAttributeGroup) * list->capacity);
    if (list->entries == NULL) {
      perror("Couldn't create list\n");
      return NULL;
    }
  }

  // check length
  if (list->length == list->capacity) {
    size_t new_capacity = 2 * list->capacity;
    void *temp =
        realloc(list->entries, sizeof(IndexAttributeGroup) * new_capacity);
    if (temp) {
      list->entries = temp;
      list->capacity = new_capacity;
    } else {
      perror("Couldn't create list\n");
      return NULL;
    }
  }

  return &list->entries[list->length++];
}

IndexAttribute *index_attribute_new_attribute(IndexAttributeGroup *list) {

  // check entries existence
  if (list->entries == NULL) {
    list->capacity = INDEX_DEFAULT_CAPACITY;
    list->length = 0;
    list->entries = malloc(sizeof(IndexAttribute) * list->capacity);
    if (list->entries == NULL) {
      perror("Couldn't create list\n");
      return NULL;
    }
  }

  // check length
  if (list->length == list->capacity) {
    size_t new_capacity = 2 * list->capacity;
    void *temp = realloc(list->entries, sizeof(IndexAttribute) * new_capacity);
    if (temp) {
      list->entries = temp;
      list->capacity = new_capacity;
    } else {
      perror("Couldn't create list\n");
      return NULL;
    }
  }

  return &list->entries[list->length++];
}

void index_attribute_from_line(const char *line, void *data) {
  VertexIndexCallbackDescriptor *cast_data =
      (VertexIndexCallbackDescriptor *)data;

  size_t prefix_len = strlen(INDEX_ATTRIBUTE_LINE_PREFIX);
  size_t line_len = strlen(line);
  // retrieve values from line
  char values[line_len];
  strncpy(values, &line[prefix_len], line_len);

  // add new index group to list and populate it
  IndexAttributeGroup *new_group = index_attribute_new_group(cast_data->list);

  if (new_group)
    index_attribute_insert_group(values, new_group);
}

/**
   Traverse the file and group index in the list
   Index List
      '- Index Group 0
      '- Index Group 1
      '- Index Group n (= index per face)
            '- Index Attribute 0
            '- Index Attribute 1
            '- Index Attribute n (= position && uv && normal)
 */
void index_attribute_cache(FILE *file, IndexAttributeList *list) {

  file_read_line_prefix(file, "f ", index_attribute_from_line,
                        &(VertexIndexCallbackDescriptor){
                            .list = list,
                        });
}

/**
   OBJ only provide index per face but doesn't build triangle.
   The function adjust Index Group entries to constitute valid triangles:
   0 1 2 3  ===> 0 1 2 2 3 0
   As the Mesh Binary primarly used for simple and lowpoly mesh, the function
   use a dummy Fan method to triangulate the polygon, it's thus the modeler
   responsibility to ensure the model has valid polygons count per faces
   (i.e.NGons could create unwanted topology)
 */
int index_attribute_triangulate(IndexAttributeList *list) {

  for (size_t i = 0; i < list->length; i++) {

    IndexAttributeGroup *group = &list->entries[i];
    // already triangle
    if (group->length < 4)
      return 2;

    size_t capacity = (group->length - 2) * 3;
    IndexAttributeGroup new_group = {
        .entries = malloc(sizeof(IndexAttribute) * capacity),
        .capacity = capacity,
        .length = 0,
    };

    if (!new_group.entries) {
      perror("Couldn't create new group attrubute\n");
      return 1;
    }

    // fan triangle
    IndexAttribute *A = &group->entries[0];
    for (size_t a = 1; a < group->length - 1; a++) {
      IndexAttribute *attributes = &group->entries[a];
      IndexAttribute *B = &group->entries[a];
      IndexAttribute *C = &group->entries[a + 1];
      memcpy(&new_group.entries[new_group.length++], A, sizeof(IndexAttribute));
      memcpy(&new_group.entries[new_group.length++], B, sizeof(IndexAttribute));
      memcpy(&new_group.entries[new_group.length++], C, sizeof(IndexAttribute));
    }

    memcpy(group->entries, new_group.entries,
           sizeof(IndexAttribute) * new_group.length);
    group->length = new_group.length;
    group->capacity = new_group.capacity;
  }
  return 0;
}

/**
   Retrieve the index position in each group entries and output it in the
   destination;

 */
void index_attribute_position_list(IndexAttributeGroup *list,
                                   mbin_index_t *dest, size_t *length,
                                   size_t *typesize) {}

/**
   Read each index group from the list,
 */
int index_attribute_compose_from_vertex(IndexAttributeList *index_list,
                                        VertexAttributeList **attr_list,
                                        mbin_vertex_t *dest) {

  // Store unique index combination in hash list along with their vertex
  for (size_t i = 0; i < index_list->length; i++) {

    IndexAttributeGroup *group = &index_list->entries[i];
    VertexHashTable table;
    vhash_create(&table, group->length * 10);

    // attributes
    for (size_t g = 0; g < group->length; g++) {

      IndexAttribute *attributes = &group->entries[g];

      // create the vertex list based on the index attributes
      mbin_vertex_t vertices[VERTEX_STRIDE];
      size_t offset = 0;

      // define position
      size_t p = attributes->position;
      VertexAttributeList *p_list = attr_list[0];
      memcpy(vertices + offset, &p_list->entries[p],
             p_list->dimension * sizeof(mbin_vertex_t));
      offset += p_list->dimension;

      // define normal
      size_t n = attributes->normal;
      VertexAttributeList *n_list = attr_list[1];
      memcpy(vertices + offset, &n_list->entries[n],
             n_list->dimension * sizeof(mbin_vertex_t));
      offset += n_list->dimension;

      // define color
      memcpy(vertices + offset, (mbin_vertex_t[3]){0.0f, 0.0f, 0.0f},
             3 * sizeof(mbin_vertex_t));
      offset += 3;

      // define uv
      size_t u = attributes->uv;
      VertexAttributeList *u_list = attr_list[2];
      memcpy(vertices + offset, &u_list->entries[u],
             u_list->dimension * sizeof(mbin_vertex_t));

      // insert vertices to hash table
      vhash_insert(&table, vertices);
    }
  }

  return 0;
}
