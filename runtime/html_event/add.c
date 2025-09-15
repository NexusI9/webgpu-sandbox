#include "add.h"

#include <stdio.h>
#include <string.h>
#include <emscripten/html5.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "utils/system.h"
#include "core.h"
#include "listener.h"

// Listener Flags
static inline uint8_t html_event_listener_flag(HTMLEventType);
static inline void html_event_listener_flag_set(HTMLEventType);
static inline bool html_event_has_listener(HTMLEventType);
static inline void html_event_check_callback(HTMLEventType, void *);

// Event lists inserts
static HTMLEventStatus html_event_insert(HTMLEventVoid *event, void **entries,
                                         size_t *length, size_t *capacity,
                                         size_t type_size,
                                         HTMLEventType event_type,
                                         void *event_callback);

uint8_t html_event_listener_flag(HTMLEventType type) { return 1u << type; }
bool html_event_has_listener(HTMLEventType type) {
  return (g_html_event.listener_flags & html_event_listener_flag(type)) != 0;
}

void html_event_listener_flag_set(HTMLEventType type) {
  g_html_event.listener_flags |= html_event_listener_flag(type);
}

/**
   ▗▄▖ ▗▄▄▄ ▗▄▄▄
  ▐▌ ▐▌▐▌  █▐▌  █
  ▐▛▀▜▌▐▌  █▐▌  █
  ▐▌ ▐▌▐▙▄▄▀▐▙▄▄▀

  Dispatch the events callbacks to the global lists
 */

void html_event_check_callback(HTMLEventType type, void *event_callback) {

  // init event listener according to event type
  // (struggle to set the event listener dynamically so use a "type based"
  // approach instead)

  bool has_listener = html_event_has_listener(type);

  // return if event listener already set
  if (has_listener)
    return;

  // else set listener flag as active
  html_event_listener_flag_set(type);

  // define HTML listener callback based on type
  switch (type) {

  case HTMLEventType_KeyDown:
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL,
                                    false, event_callback);
    break;

  case HTMLEventType_KeyUp:
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, NULL, false,
                                  event_callback);
    break;

  case HTMLEventType_MouseMove:
    emscripten_set_mousemove_callback(g_html_event.target, NULL, false,
                                      event_callback);
    break;

  case HTMLEventType_MouseDown:
    emscripten_set_mousedown_callback(g_html_event.target, NULL, false,
                                      event_callback);
    break;

  case HTMLEventType_MouseUp:
    emscripten_set_mouseup_callback(g_html_event.target, NULL, false,
                                    event_callback);
    break;

  case HTMLEventType_Wheel:
    emscripten_set_wheel_callback(g_html_event.target, NULL, false,
                                  event_callback);
    break;

  default:
    break;
  }
}

HTMLEventStatus html_event_insert(HTMLEventVoid *event, void **entries,
                                  size_t *length, size_t *capacity,
                                  size_t type_size, HTMLEventType event_type,
                                  void *event_callback) {

  html_event_check_callback(event_type, event_callback);

  // check html event list initialized
  if (*entries == NULL) {

    // allocate new list
    *capacity = HTML_EVENT_DEFAULT_CAPACITY;
    *length = 0;
    *entries = malloc(type_size * (*capacity));

    if (*entries == NULL) {
      VERBOSE_ERROR("Couldn't allocate html event.");
      *capacity = 0;
      return HTMLEventStatus_AllocFail;
    }
  }

  // check html event list capacity
  if (*length == *capacity) {

    size_t new_capacity = 2 * (*capacity);
    void *temp = realloc(*entries, new_capacity * type_size);

    if (temp != NULL) {
      *entries = temp;
      *capacity = new_capacity;
    } else {
      VERBOSE_ERROR("Coudln't reallocate html event.");
      return HTMLEventStatus_AllocFail;
    }
  }

  // once we've checked if event list can store new event, we dynamically
  // retrieve entry pointer position at the byte level
  HTMLEventVoid *cast_entry =
      (HTMLEventVoid *)((char *)(*entries) + (*length) * type_size);

  // append new event object to list
  memcpy(cast_entry, event, type_size);

  // allocate data on stack if any data AND size
  // if size if 0, it won't replace the data with allocated one
  if (event->data != NULL && event->size) {

    html_event_data stored_data = malloc(event->size);

    if (stored_data != NULL) {
      cast_entry->data = stored_data;
      memcpy(cast_entry->data, event->data, event->size);
    } else {
      VERBOSE_ERROR("Coudln't allocate memory for html event data.");
      return HTMLEventStatus_AllocFail;
    }
  }

  (*length)++;

  return HTMLEventStatus_Success;
}

/**
   Add a mouse down event to the relative list.
 */
HTMLEventStatus html_event_add_mouse_down(HTMLEventMouse *event) {

  void *entries = &g_html_event.mouse_down.entries;
  size_t *length = &g_html_event.mouse_down.length;
  size_t *capacity = &g_html_event.mouse_down.capacity;
  size_t type_size = sizeof(HTMLEventMouse);
  HTMLEventType event_type = HTMLEventType_MouseDown;
  void *event_callback = html_event_listener_mouse_down;

  return html_event_insert(
      &(HTMLEventVoid){
          .callback = (void *)event->callback,
          .destructor = (void *)event->destructor,
          .data = (void *)event->data,
          .size = event->size,
          .owner = event->owner,
      },
      entries, length, capacity, type_size, event_type, event_callback);
}

/**
   Add a mouse up event to the relative list.
 */
HTMLEventStatus html_event_add_mouse_up(HTMLEventMouse *event) {

  void *entries = &g_html_event.mouse_up.entries;
  size_t *length = &g_html_event.mouse_up.length;
  size_t *capacity = &g_html_event.mouse_up.capacity;
  size_t type_size = sizeof(HTMLEventMouse);
  HTMLEventType event_type = HTMLEventType_MouseUp;
  void *event_callback = html_event_listener_mouse_up;

  return html_event_insert(
      &(HTMLEventVoid){
          .callback = (void *)event->callback,
          .destructor = (void *)event->destructor,
          .data = (void *)event->data,
          .size = event->size,
          .owner = event->owner,
      },
      entries, length, capacity, type_size, event_type, event_callback);
}

/**
   Add a mouse move event to the relative list
 */
HTMLEventStatus html_event_add_mouse_move(HTMLEventMouse *event) {

  void *entries = &g_html_event.mouse_move.entries;
  size_t *length = &g_html_event.mouse_move.length;
  size_t *capacity = &g_html_event.mouse_move.capacity;
  size_t type_size = sizeof(HTMLEventMouse);
  HTMLEventType event_type = HTMLEventType_MouseMove;
  void *event_callback = html_event_listener_mouse_move;

  return html_event_insert(
      &(HTMLEventVoid){
          .callback = (void *)event->callback,
          .destructor = (void *)event->destructor,
          .data = (void *)event->data,
          .size = event->size,
          .owner = event->owner,
      },
      entries, length, capacity, type_size, event_type, event_callback);
}

/**
   Add a mouse wheel event to the relative list
 */
HTMLEventStatus html_event_add_wheel(HTMLEventWheel *event) {

  void *entries = &g_html_event.wheel.entries;
  size_t *length = &g_html_event.wheel.length;
  size_t *capacity = &g_html_event.wheel.capacity;
  size_t type_size = sizeof(HTMLEventWheel);
  HTMLEventType event_type = HTMLEventType_Wheel;
  void *event_callback = html_event_listener_wheel;

  return html_event_insert(
      &(HTMLEventVoid){
          .callback = (void *)event->callback,
          .destructor = (void *)event->destructor,
          .data = (void *)event->data,
          .size = event->size,
          .owner = event->owner,
      },
      entries, length, capacity, type_size, event_type, event_callback);
}

/**
   Add a key down  event to the relative list
 */
HTMLEventStatus html_event_add_key_down(HTMLEventKey *event) {

  void *entries = &g_html_event.key_down.entries;
  size_t *length = &g_html_event.key_down.length;
  size_t *capacity = &g_html_event.key_down.capacity;
  size_t type_size = sizeof(HTMLEventKey);
  HTMLEventType event_type = HTMLEventType_KeyDown;
  void *event_callback = html_event_listener_key_down;

  return html_event_insert(
      &(HTMLEventVoid){
          .callback = (void *)event->callback,
          .destructor = (void *)event->destructor,
          .data = (void *)event->data,
          .size = event->size,
          .owner = event->owner,
      },
      entries, length, capacity, type_size, event_type, event_callback);
}

/**
   Add a key up event to the relative list
 */
HTMLEventStatus html_event_add_key_up(HTMLEventKey *event) {

  void *entries = &g_html_event.key_up.entries;
  size_t *length = &g_html_event.key_up.length;
  size_t *capacity = &g_html_event.key_up.capacity;
  size_t type_size = sizeof(HTMLEventKey);
  HTMLEventType event_type = HTMLEventType_KeyUp;
  void *event_callback = html_event_listener_key_up;

  return html_event_insert(
      &(HTMLEventVoid){
          .callback = (void *)event->callback,
          .destructor = (void *)event->destructor,
          .data = (void *)event->data,
          .size = event->size,
          .owner = event->owner,
      },
      entries, length, capacity, type_size, event_type, event_callback);
}
