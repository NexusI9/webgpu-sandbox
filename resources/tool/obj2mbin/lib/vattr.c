#include "vattr.h"
#include <stdio.h>
#include <string.h>

void vertex_attribute_list_print(VertexAttributeList *list) {
  for (size_t l = 0; l < list->length; l++) {
    printf("%f\t", list->entries[l]);
    if (l % list->dimension == list->dimension - 1)
      printf("\n");
  }

  printf("\n");
}

int vertex_attribute_list_insert(mbin_vertex_t value,
                                 VertexAttributeList *list) {

  // init list
  if (list->entries == NULL) {
    list->entries = malloc(sizeof(mbin_vertex_t) * list->capacity);
    if (list->entries == NULL) {
      perror("Couldn't create list\n");
      return 1;
    }
  }

  // expand list
  if (list->length == list->capacity) {

    size_t new_capacity = 2 * list->capacity;
    void *temp = realloc(list->entries, sizeof(mbin_vertex_t) * new_capacity);

    if (temp) {
      list->entries = temp;
      list->capacity = new_capacity;
    } else {
      perror("Couldn't realloc list\n");
      return 1;
    }
  }

  list->entries[list->length++] = value;

  return 0;
}

/**
   Split a line into float values and insert it in the given list.
 */
void vertex_attribute_from_line(const char *line, void *data) {

  VertexAttributeCallbackDescriptor *desc =
      (VertexAttributeCallbackDescriptor *)data;

  size_t prefix_len = strlen(desc->list->prefix);
  // retrieve values from line
  char values[sizeof(line)];
  strncpy(values, &line[prefix_len], sizeof(line) - prefix_len);

  // split values
  char *token = strtok(values, " ");
  while (token) {
    // convert char to float
    float value = strtof(token, NULL);
    vertex_attribute_list_insert(value, desc->list);
    token = strtok(0, " ");
  }
}
