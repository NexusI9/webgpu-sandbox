#include "vattr.h"
#include "file.h"
#include "mbin.h"
#include <stdio.h>
#include <string.h>

void mbin_vertex_attribute_print(VertexAttributeList *list) {

  printf("Attributes: %s\n", list->label);
  printf("Length: %lu\n", list->length);
  printf("Capacity: %lu\n", list->capacity);
  printf("Data: \n");
  for (size_t l = 0; l < list->length; l++) {
    printf("%f\t", list->entries[l]);
    if (l % list->dimension == list->dimension - 1)
      printf("\n");
  }
  printf("\n");
}

VertexAttributeListStatus
mbin_vertex_attribute_list_insert(VertexAttributeList *list,
                                  mbin_vertex_t *value, size_t count) {

  // init list
  if (list->entries == NULL) {
    list->entries = malloc(sizeof(mbin_vertex_t) * list->capacity);
    if (list->entries == NULL) {
      perror("Couldn't create list\n");
      return VertexAttributeListStatus_AllocFail;
    }
  }

  // expand list
  if (list->length == list->capacity ||
      list->length + count >= list->capacity) {
    size_t new_capacity = 2 * (list->capacity + count);
    void *temp = realloc(list->entries, sizeof(mbin_vertex_t) * new_capacity);

    if (temp) {
      list->entries = temp;
      list->capacity = new_capacity;
    } else {
      perror("Couldn't realloc list\n");
      return VertexAttributeListStatus_AllocFail;
    }
  }

  memcpy(&list->entries[list->length], value, count * sizeof(mbin_vertex_t));
  list->length += count;

  return VertexAttributeListStatus_Success;
}

/**
   Split a line into float values and insert it in the given list.
 */
void mbin_vertex_attribute_from_line(const char *line, void *data) {

  VertexAttributeCallbackDescriptor *desc =
      (VertexAttributeCallbackDescriptor *)data;

  size_t prefix_len = strlen(desc->list->prefix);
  size_t line_len = strlen(line);
  size_t content_len = line_len - prefix_len;
  // retrieve values from line
  char values[content_len + 1]; // + null term

  memcpy(values, &line[prefix_len], content_len);
  values[content_len] = '\0'; // null terminate

  // split values
  char *token = strtok(values, VERTEX_SEPARATOR);
  while (token) {
    // convert char to float
    float value = strtof(token, NULL);
    mbin_vertex_attribute_list_insert(desc->list, &value, 1);
    token = strtok(NULL, VERTEX_SEPARATOR);
  }
}

void mbin_vertex_attribute_free(VertexAttributeList *list) {

  if (list->entries) {
    free(list->entries);
    list->entries = NULL;
  }

  if (list->prefix) {
    free(list->prefix);
    list->prefix = NULL;
  }
}

VertexAttributeListStatus
mbin_vertex_attribute_copy(VertexAttributeList *src,
                           VertexAttributeList *dest) {

  memcpy(dest, src, sizeof(VertexAttributeList));

  dest->prefix = src->prefix;
  dest->entries = malloc(dest->capacity * sizeof(mbin_vertex_t));

  if (dest->entries == NULL) {
    perror("Couldn't copy list\n");

    dest->capacity = 0;
    dest->length = 0;
    dest->dimension = 0;
    dest->prefix = NULL;
    
    return VertexAttributeListStatus_AllocFail;
  }

  memcpy(dest->entries, src->entries, dest->length * sizeof(mbin_vertex_t));

  return VertexAttributeListStatus_Success;
}

void mbin_vertex_attribute_set_line_uv(VertexAttributeList *list) {

  const size_t new_uv_length = 8;
  const float A_mul = 1.0f;
  const float B_mul = -1.0f;

  static mbin_vertex_t uv_line_data[] = {
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

  list->label = "uv";
  list->offset = VertexAttributeOffset_Uv;
  list->dimension = VertexAttributeDimension_Uv;
  list->capacity = new_uv_length;
  list->length = new_uv_length;
  list->entries = uv_line_data;
}
