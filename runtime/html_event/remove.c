#include "remove.h"
#include "core.h"
#include <string.h>

static void html_event_remove(void *, size_t *, size_t, size_t);
static void html_event_traverse_remove(void *, size_t *, size_t, size_t);

void html_event_remove(void *entries, size_t *length, size_t index,
                       size_t type_size) {

  if (index >= *length)
    return;

  char *bytes = (char *)entries;
  HTMLEventVoid *event = (HTMLEventVoid *)(bytes + index * type_size);

  // free data
  if (event->data != NULL) {
    free(event->data);
    event->data = NULL;
    event->size = 0;
  }

  // shift array bytes
  if (index < *length - 1) {
    memmove(&bytes[index * type_size], &bytes[(index + 1) * type_size],
            (*length - index - 1) * type_size);
  }

  (*length)--;
}

/**
   Go through the event list and check each event owner if it matches the given
   id
 */
void html_event_traverse_remove(void *entries, size_t *length, size_t id,
                                size_t type_size) {

  // C doesn't allow pointer arithmetic on void* array, so converts to bytes to
  // manipulate it
  char *bytes = (char *)entries;

  for (size_t i = 0; i < *length;) {
    HTMLEventVoid *event = (HTMLEventVoid *)(bytes + i * type_size);
    if (event->owner == id)
      // do not increment i automaticall cause we shift item to left
      // (so no point incrementing, next element becomes "in place")
      html_event_remove(entries, length, i, type_size);
    else
      // only increment when not removed
      i++;
  }
}

// mouse events
void html_event_remove_mouse_down(id_t id) {

  void *entries = g_html_event.mouse_down.entries;
  size_t *length = &g_html_event.mouse_down.length;
  size_t type_size = sizeof(HTMLEventMouse);

  html_event_traverse_remove(entries, length, id, type_size);
}

void html_event_remove_mouse_move(id_t id) {

  void *entries = g_html_event.mouse_move.entries;
  size_t *length = &g_html_event.mouse_move.length;
  size_t type_size = sizeof(HTMLEventMouse);

  html_event_traverse_remove(entries, length, id, type_size);
}

// wheel events
void html_event_remove_wheel(id_t id) {

  void *entries = g_html_event.wheel.entries;
  size_t *length = &g_html_event.wheel.length;
  size_t type_size = sizeof(HTMLEventWheel);

  html_event_traverse_remove(entries, length, id, type_size);
}

// key events
void html_event_remove_key_down(id_t id) {

  void *entries = g_html_event.key_down.entries;
  size_t *length = &g_html_event.key_down.length;
  size_t type_size = sizeof(HTMLEventKey);

  html_event_traverse_remove(entries, length, id, type_size);
}
void html_event_remove_key_up(id_t id) {

  void *entries = g_html_event.key_up.entries;
  size_t *length = &g_html_event.key_up.length;
  size_t type_size = sizeof(HTMLEventKey);

  html_event_traverse_remove(entries, length, id, type_size);
}
