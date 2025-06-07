
#include "vindex.h"
#include "buffer.h"
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
static int index_attribute_insert_group(char *, IndexAttributeGroup *,
                                        const char *);
static void index_attribute_from_line(const char *, void *);


void index_attribute_print(const IndexAttributeList *list) {
  if (list->length == 0)
    return;
  printf("Index: \n");
  for (size_t g = 0; g < list->length; g++) {
    for (size_t i = 0; i < list->entries[g].length; i++) {
      IndexAttribute *attr = &list->entries[g].entries[i];
      printf("%d,%d,%d\t", attr->position, attr->normal, attr->uv);
    }
    printf("\n");
  }
}

int index_attribute_insert_group(char *line, IndexAttributeGroup *list,
                                 const char *pattern) {

  // split values and push them into the current list
  // "1/3/4 1/9/4 3/2/1" => [ [1/3/4] , [1/9/4] , [3/2/1] ]
  char *index_group = strtok(line, VINDEX_GROUP_SEPARATOR);

  while (index_group) {

    // store attributes in an array since pattern may no necessarily match 3,
    // need 0 as initial value
    const size_t attr_count = 3;
    mbin_index_t idx_attr[attr_count] = {0, 0, 0}; // position/ uv / normal
    mbin_index_t *iPos = &idx_attr[0];
    mbin_index_t *iUv = &idx_attr[1];
    mbin_index_t *iNrm = &idx_attr[2];

    int scan_length = sscanf(index_group, pattern, iPos, iUv, iNrm);

    // get new entry pointer
    IndexAttribute *new_attribute = index_attribute_new_attribute(list);
    if (new_attribute) {
      mbin_index_t *new_attr_idx[3] = {
          &new_attribute->position,
          &new_attribute->uv,
          &new_attribute->normal,
      };

      // parallel assign idx_attr to new_attr_idx
      for (size_t i = 0; i < scan_length; i++)
        // -1 cause obj index starts at 1, but array c 0
        *new_attr_idx[i] = idx_attr[i] - 1;
    }

    index_group = strtok(0, VINDEX_GROUP_SEPARATOR);
  }

  return 0;
}

IndexAttributeGroup *index_attribute_new_group(IndexAttributeList *list) {

  // check entries existence
  if (list->entries == NULL) {
    list->capacity = VINDEX_DEFAULT_CAPACITY;
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
    list->capacity = VINDEX_DEFAULT_CAPACITY;
    list->length = 0;
    list->entries = malloc(sizeof(IndexAttribute) * list->capacity);
    if (list->entries == NULL) {
      perror("Couldn't create list\n");
      return NULL;
    }
  }

  // check capacity reach
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

  size_t prefix_len = strlen(VINDEX_ATTRIBUTE_LINE_PREFIX);
  size_t line_len = strlen(line);
  // retrieve values from line
  char values[line_len];
  strncpy(values, &line[prefix_len], line_len);

  // add new index group to list and populate it
  IndexAttributeGroup *new_group = index_attribute_new_group(cast_data->list);

  if (new_group)
    index_attribute_insert_group(values, new_group, cast_data->pattern);
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
void index_attribute_cache(FILE *file, IndexAttributeList *list,
                           const char *prefix, const char *pattern) {

  // read faces
  file_read_line_prefix(
      file, prefix, index_attribute_from_line,
      &(VertexIndexCallbackDescriptor){.list = list, .pattern = pattern});
}

/**
   OBJ only provide index per face but doesn't build triangle.
   The function adjust Index Group entries to constitute valid triangles:
   0 1 2 3  ===> 0 1 2 2 3 1
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
      return VINDEX_SUCCESS;

    size_t capacity = (group->length - 2) * 3;
    IndexAttributeGroup new_group = {
        .entries = malloc(sizeof(IndexAttribute) * capacity),
        .capacity = capacity,
        .length = 0,
    };

    if (!new_group.entries) {
      perror("Couldn't create new group attrubute\n");
      return VINDEX_ALLOC_FAILURE;
    }

    // fan triangle
    IndexAttribute *A = &group->entries[0];
    for (size_t a = 1; a < group->length - 1; a++) {
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
  return VINDEX_SUCCESS;
}

/**
   Retrieve the index position in each group entries and output it in the
   destination.
 */
void index_attribute_position_list(IndexAttributeGroup *list,
                                   mbin_index_t *dest, size_t *length,
                                   size_t *typesize) {}


/**
   Lines index list follow this specific pattern:

   .----------------.----------------.
   |     group 0    |     group 1    |
   |----------------+----------------|
   |  pA / nA / tA  |  pB / nB / tB  |
   '----------------'----------------'
   For lines vertex the normal (n) gets replace by the opposition point
   position. Hence this function replace the normal index by the respective
   opposite position value:

   pA / pB / tA   pB / pA / tB

   We simply override the normals index because we previously copied the
   vertex positions list into the normal list for our cached *lines* vertex
   attributes.
   Copying the postion into the normal and by replacing the
   normal index by the position index allows to reference to the same
   "position space" during the mapping phase without creating dedicating
   functions:

       pX     |      nX                pX       |      pY
       |      |      |                 |        |      |
   p1 p2 p3   |  n1 n2 n3          p1 p2 p3     |   p1 p2 p3
   p4 p5 p6   |  n4 n5 n6   ====>  p4 p5 p6     |   p4 p5 p6
   p7 p8 ..   |  n7 n8 ..          p7 p8 ..     |   p7 p8 ..

 */
void index_attribute_line_set_opposite(IndexAttributeList *list) {

  for (size_t i = 0; i < list->length; i++) {

    IndexAttribute *p1 = &list->entries[i].entries[0];
    IndexAttribute *p2 = &list->entries[i].entries[1];

    p1->normal = p2->position;
    p2->normal = p1->position;
  }
}

void index_attribute_copy(IndexAttribute *src, IndexAttribute *dest) {
  memcpy(dest, src, sizeof(IndexAttribute));
}

/**
   Initially, OBJ lines only have 2 points, however the engine's line take 4
   points as to create a wireframe illusion (i.e. a very thin quad).
   The difference is that the last two points' side attribute are the
   inverse of the two first ones (1 vs -1), "side" data allows to give hints
   to the shader on which direction the vertex shall move.

   The below function create two attributes copy and assign a uv index to 1
 */
void index_attribute_line_set_doublon(IndexAttributeList *list) {
  for (size_t i = 0; i < list->length; i++) {

    IndexAttributeGroup *current_group = &list->entries[i];

    IndexAttribute new_attributes[2];
    for (size_t c = 0; c < 2; c++) {
      IndexAttribute *src_attribute = &current_group->entries[c];
      // IndexAttribute *new_attribute =
      //     index_attribute_new_attribute(current_group);
      //  copy source (0 & 1) to new attribute
      IndexAttribute *new_attribute = &new_attributes[c];

      index_attribute_copy(src_attribute, new_attribute);
      // set uv to 1 (i.e. opposite side)
      new_attribute->uv = 1;
    }

    IndexAttribute *new_attribute_a =
        index_attribute_new_attribute(current_group);
    index_attribute_copy(&new_attributes[1], new_attribute_a);

    IndexAttribute *new_attribute_b =
        index_attribute_new_attribute(current_group);
    index_attribute_copy(&new_attributes[0], new_attribute_b);
  }
}
