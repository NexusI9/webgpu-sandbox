
#include "vindex.h"
#include "file.h"
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
    list->entries = malloc(sizeof(IndexAttributeGroup *) * list->capacity);
    if (list->entries == NULL) {
      perror("Couldn't create list\n");
      return NULL;
    }
  }

  // check length
  if (list->length == list->capacity) {
    size_t new_capacity = 2 * list->capacity;
    void *temp = realloc(list->entries, sizeof(IndexAttributeGroup *));
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
    list->entries = malloc(sizeof(IndexAttribute *) * list->capacity);
    if (list->entries == NULL) {
      perror("Couldn't create list\n");
      return NULL;
    }
  }

  // check length
  if (list->length == list->capacity) {
    size_t new_capacity = 2 * list->capacity;
    void *temp = realloc(list->entries, sizeof(IndexAttribute *));
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
                        &(VertexIndexCallbackDescriptor){.list = list});

#ifdef VERBOSE
  index_attribute_print(list);
#endif
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
void index_group_triangulate(IndexAttributeGroup *group) {
  if (group->length < 4)
    return;
}

void index_attribute_position_list(IndexAttributeGroup *list,
                                   mbin_index_t *dest, size_t *length,
                                   size_t *typesize) {}

void index_attribute_compose_from_vertex(IndexAttributeGroup *index_list,
                                         VertexAttributeList *attr_list,
                                         mbin_vertex_t *dest, size_t *length,
                                         size_t *typesize) {}
