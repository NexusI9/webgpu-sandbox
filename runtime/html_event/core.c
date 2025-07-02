#include "core.h"
#include "emscripten/html5.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

HTMLEvent g_html_event = {0};

// Listeners
static int html_event_mouse_listener(HTMLEventMouse *);
static int html_event_wheel_listener(HTMLEventWheel *);
static int html_event_key_listener(HTMLEventKey *);

// Event lists inserts
static int html_event_insert(HTMLEventVoid *event, void **entries,
                             size_t *length, size_t *capacity, size_t type_size,
                             emscripten_event_listener event_listener,
                             void *event_callback);

static int html_event_mouse_insert(HTMLEventMouse *);
static int html_event_wheel_insert(HTMLEventWheel *);
static int html_event_key_insert(HTMLEventKey *);
void html_event_init(html_event_target target) {
  g_html_event.target = strdup(target);
}

int html_event_insert(HTMLEventVoid *event, void **entries, size_t *length,
                      size_t *capacity, size_t type_size,
                      emscripten_event_listener event_listener,
                      void *event_callback) {

  // check initialized
  if (*entries == NULL) {

    // allocate new list
    *capacity = HTML_EVENT_DEFAULT_CAPACITY;
    *length = 0;
    *entries = malloc(type_size * (*capacity));

    if (*entries != NULL) {
      // add event listener
      event_listener(g_html_event.target, NULL, false, event_callback);
    } else {
      perror("Coudln't allocate html event.\n");
      *capacity = 0;
      return HTML_EVENT_ALLOC_FAIL;
    }
  }

  // check capacity
  if (*length == *capacity) {

    size_t new_capacity = 2 * (*capacity);
    void *temp = realloc(*entries, new_capacity * type_size);

    if (temp != NULL) {
      *entries = temp;
      *capacity = new_capacity;
    } else {
      perror("Coudln't reallocate html event.\n");
      return HTML_EVENT_ALLOC_FAIL;
    }
  }

  // append new event object to list
  memcpy((char *)(*entries) + (*length) * type_size, event, type_size);
  (*length)++;

  return HTML_EVENT_SUCCESS;
}

int html_event_mouse_insert(HTMLEventMouse *event) {

  return HTML_EVENT_SUCCESS;
}

/**
   Add a mouse click event to the relative list.
 */
int html_event_add_mouse_click(HTMLEventMouse *event) {

  return HTML_EVENT_SUCCESS;
}

/**
   Add a mouse move event to the relative list
 */
int html_event_add_mouse_move(HTMLEventMouse *event) {

  return HTML_EVENT_SUCCESS;
}

/**
   Add a mouse wheel event to the relative list
 */
int html_event_add_wheel(HTMLEventWheel *event) { return HTML_EVENT_SUCCESS; }

/**
   Add a key down  event to the relative list
 */
int html_event_add_key_down(HTMLEventKey *event) { return HTML_EVENT_SUCCESS; }

/**
   Add a key up event to the relative list
 */
int html_event_add_key_up(HTMLEventKey *event) { return HTML_EVENT_SUCCESS; }
