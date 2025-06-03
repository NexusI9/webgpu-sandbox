#include "vattr.h"
#include "file.h"
#include "mbin.h"
#include <stdio.h>
#include <string.h>

void vertex_attribute_print(VertexAttributeList *list) {

  if (list->length == 0)
    return;
  
  printf("Attributes: \n");
  for (size_t l = 0; l < list->length; l++) {
    printf("%f\t", list->entries[l]);
    if (l % list->dimension == list->dimension - 1)
      printf("\n");
  }
  printf("\n");
}

int vertex_attribute_list_insert(VertexAttributeList *list,
                                 mbin_vertex_t *value, size_t count) {

  // init list
  if (list->entries == NULL) {
    list->entries = malloc(sizeof(mbin_vertex_t) * list->capacity);
    if (list->entries == NULL) {
      perror("Couldn't create list\n");
      return VERTEX_ATTRIBUTE_LIST_ALLOC_FAIL;
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
      return VERTEX_ATTRIBUTE_LIST_ALLOC_FAIL;
    }
  }

  memcpy(&list->entries[list->length], value, count * sizeof(mbin_vertex_t));
  list->length += count;

  return VERTEX_ATTRIBUTE_LIST_SUCCESS;
}

/**
   Split a line into float values and insert it in the given list.
 */
void vertex_attribute_from_line(const char *line, void *data) {

  VertexAttributeCallbackDescriptor *desc =
      (VertexAttributeCallbackDescriptor *)data;

  size_t prefix_len = strlen(desc->list->prefix);
  size_t line_len = strlen(line);
  // retrieve values from line
  char values[line_len];
  strncpy(values, &line[prefix_len], line_len - prefix_len);
  // split values
  char *token = strtok(values, VERTEX_SEPARATOR);
  while (token) {
    // convert char to float
    float value = strtof(token, NULL);
    vertex_attribute_list_insert(desc->list, &value, 1);
    token = strtok(0, VERTEX_SEPARATOR);
  }
}

void vertex_attribute_cache(FILE *file, VertexAttributeList **list) {

  // cache attributes in their respective array
  for (int v = 0; v < 3; v++) {
    VertexAttributeList *list_attr = list[v];
    file_read_line_prefix(file, list_attr->prefix, vertex_attribute_from_line,
                          &(VertexAttributeCallbackDescriptor){
                              .list = list_attr,
                          });
  }
}

void vertex_attribute_free(VertexAttributeList *list) {

  if (list->entries) {
    free(list->entries);
    list->entries = NULL;
  }

  if (list->prefix) {
    free(list->prefix);
    list->prefix = NULL;
  }
}

int vertex_attribute_copy(VertexAttributeList *src, VertexAttributeList *dest) {
  dest->capacity = src->capacity;
  dest->length = src->length;
  dest->dimension = src->dimension;
  dest->prefix = strdup(src->prefix);

  dest->entries = malloc(dest->capacity * sizeof(mbin_vertex_t));

  if (dest->entries == NULL) {
    perror("Couldn't copy list\n");

    dest->capacity = 0;
    dest->length = 0;
    dest->dimension = 0;

    free(dest->prefix);
    dest->prefix = NULL;
    return VERTEX_ATTRIBUTE_LIST_ALLOC_FAIL;
  }

  memcpy(dest->entries, src->entries, dest->length * sizeof(mbin_vertex_t));

  return VERTEX_ATTRIBUTE_LIST_SUCCESS;
}
