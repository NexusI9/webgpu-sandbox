
#include "vindex.h"
#include "buffer.h"
#include "file.h"
#include "mbin.h"
#include "vattr.h"
#include "vhash.h"
#include <stddef.h>
#include <stdint.h>
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
static index_attribute *index_attribute_new_attribute(IndexAttributeGroup *);
static VIndexStatus index_attribute_insert_group(char *, IndexAttributeGroup *,
                                                 const char *);
static void index_attribute_from_line(const char *, void *);

void index_attribute_print(const IndexAttributeList *list) {
  if (list->length == 0)
    return;
  printf("Index: \n");
  for (size_t g = 0; g < list->length; g++) {
    for (size_t i = 0; i < list->entries[g].length; i++) {

      index_attribute *attr = &list->entries[g].entries[i];
      for (VertexAttributeType a = 0; a < VERTEX_ATTRIBUTE_COUNT; a++)
        printf("%d ", (*attr)[a]);
      printf("\t");
    }
    printf("\n");
  }
}

/*
  split values and push them into the current list
  ""1/3/4 1/9/4 3/2/1" => [ [1/3/4] , [1/9/4] , [3/2/1] ]
 */
VIndexStatus index_attribute_insert_group(char *line, IndexAttributeGroup *list,
                                          const char *pattern) {

  char *index_group = strtok(line, VINDEX_GROUP_SEPARATOR);

  while (index_group) {

    // get new entry pointer
    index_attribute *new_attr = index_attribute_new_attribute(list);
    if (new_attr) {

      int scan_length = sscanf(index_group, pattern,
                               &(*new_attr)[VertexAttributeType_Position],
                               &(*new_attr)[VertexAttributeType_Uv],
                               &(*new_attr)[VertexAttributeType_Normal]);

      // decrement each index since obj index starts at 1 (instead of 0)
      // printf("%s (%s) => ", index_group, pattern);
      for (VertexAttributeType i = 0; i < VERTEX_ATTRIBUTE_COUNT; i++) {
        if ((*new_attr)[i] > 0)
          (*new_attr)[i] -= 1;

        // printf("%d ", (*new_attr)[i]);
      }
      // printf("\n");
    }
    // flush
    index_group = strtok(NULL, VINDEX_GROUP_SEPARATOR);
  }

  return VIndexStatus_Success;
}

IndexAttributeGroup *index_attribute_new_group(IndexAttributeList *list) {

  // check entries existence
  if (list->entries == NULL) {
    list->capacity = VINDEX_DEFAULT_CAPACITY;
    list->length = 0;
    list->entries = calloc(list->capacity, sizeof(IndexAttributeGroup));
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

      memset(&list->entries[list->capacity], 0,
             (new_capacity - list->capacity) * sizeof(IndexAttributeGroup));

      list->capacity = new_capacity;
    } else {
      perror("Couldn't create list\n");
      return NULL;
    }
  }

  return &list->entries[list->length++];
}

index_attribute *index_attribute_new_attribute(IndexAttributeGroup *list) {

  // check entries existence
  if (list->entries == NULL) {
    list->capacity = VINDEX_DEFAULT_CAPACITY;
    list->length = 0;
    list->entries = calloc(list->capacity, sizeof(index_attribute));
    if (list->entries == NULL) {
      perror("Couldn't create list\n");
      return NULL;
    }
  }

  // check capacity reach
  if (list->length == list->capacity) {
    size_t new_capacity = 2 * list->capacity;
    void *temp = realloc(list->entries, sizeof(index_attribute) * new_capacity);
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

/*
  Read the file attribute line ('f 3/2') and store it as in a new group.
 */
void index_attribute_from_line(const char *line, void *data) {

  VertexIndexCallbackDescriptor *cast_data =
      (VertexIndexCallbackDescriptor *)data;

  size_t prefix_len = strlen(VINDEX_ATTRIBUTE_LINE_PREFIX);
  size_t line_len = strlen(line);
  ssize_t content_len = line_len - prefix_len;

  // retrieve values from line
  char values[content_len + 1];
  memcpy(values, &line[prefix_len], content_len);
  values[content_len] = '\0';

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
VIndexStatus index_attribute_triangulate(IndexAttributeList *list) {

  for (size_t i = 0; i < list->length; i++) {

    IndexAttributeGroup *group = &list->entries[i];

    // already triangle
    if (group->length < 4)
      return VIndexStatus_Success;

    size_t capacity = (group->length - 2) * 3;
    IndexAttributeGroup new_group = {
        .entries = malloc(sizeof(index_attribute) * capacity),
        .capacity = capacity,
        .length = 0,
    };

    if (!new_group.entries) {
      perror("Couldn't create new group attrubute\n");
      return VIndexStatus_AllocFail;
    }

    // fan triangle
    index_attribute *A = &group->entries[0];
    for (size_t a = 1; a < group->length - 1; a++) {
      index_attribute *B = &group->entries[a];
      index_attribute *C = &group->entries[a + 1];
      memcpy(&new_group.entries[new_group.length++], A,
             sizeof(index_attribute));
      memcpy(&new_group.entries[new_group.length++], B,
             sizeof(index_attribute));
      memcpy(&new_group.entries[new_group.length++], C,
             sizeof(index_attribute));
    }

    // free previous entries
    free(group->entries);

    // overwrite initial group entries
    group->entries = new_group.entries;
    group->length = new_group.length;
    group->capacity = new_group.capacity;
  }
  return VIndexStatus_Success;
}

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

    index_attribute *p1 = &list->entries[i].entries[0];
    index_attribute *p2 = &list->entries[i].entries[1];

    (*p1)[VertexAttributeType_Normal] = (*p2)[VertexAttributeType_Position];
    (*p2)[VertexAttributeType_Normal] = (*p1)[VertexAttributeType_Position];
  }
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

    index_attribute new_attributes[2];
    for (size_t c = 0; c < 2; c++) {
      index_attribute *src_attribute = &current_group->entries[c];

      //  copy source (0 & 1) to new attribute
      index_attribute *new_attribute = &new_attributes[c];

      index_attribute_copy(src_attribute, new_attribute);
      // set uv to 1 (i.e. opposite side)
      // patter A / B/ A / B
      (*src_attribute)[VertexAttributeType_Uv] = c * 2;
      (*new_attribute)[VertexAttributeType_Uv] = c * 2 + 1;
    }

    // swap
    index_attribute *new_attribute_a =
        index_attribute_new_attribute(current_group);
    index_attribute_copy(&new_attributes[1], new_attribute_a);

    index_attribute *new_attribute_b =
        index_attribute_new_attribute(current_group);
    index_attribute_copy(&new_attributes[0], new_attribute_b);
  }
}
