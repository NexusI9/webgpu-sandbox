#include "core.h"
#include "emscripten/em_types.h"
#include <emscripten/html5.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// Listeners
static bool html_event_listener_mouse_move(int, const EmscriptenMouseEvent *,
                                           void *);
static bool html_event_listener_mouse_down(int, const EmscriptenMouseEvent *,
                                           void *);
static bool html_event_listener_wheel(int, const EmscriptenWheelEvent *,
                                      void *);
static bool html_event_listener_key_down(int, const EmscriptenKeyboardEvent *,
                                         void *);
static bool html_event_listener_key_up(int, const EmscriptenKeyboardEvent *,
                                       void *);

// Listener Flags
static inline unsigned int html_event_listener_flag(HTMLEventType);
static inline void html_event_listener_flag_set(HTMLEventType);
static inline bool html_event_has_listener(HTMLEventType);
static inline void html_event_check_callback(HTMLEventType, void *);

// Event lists inserts
static int html_event_insert(HTMLEventVoid *event, void **entries,
                             size_t *length, size_t *capacity, size_t type_size,
                             HTMLEventType event_type, void *event_callback);

/**

  ▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖▗▄▄▄▖
    █  ▐▛▚▖▐▌  █    █
    █  ▐▌ ▝▜▌  █    █
  ▗▄█▄▖▐▌  ▐▌▗▄█▄▖  █

 */

HTMLEvent g_html_event = {0};

void html_event_init(html_event_target target) {
  g_html_event.target = strdup(target);

  // reset lists
  g_html_event.mouse_move = (HTMLEventMouseList){0};
  g_html_event.mouse_down = (HTMLEventMouseList){0};
  g_html_event.key_down = (HTMLEventKeyList){0};
  g_html_event.key_up = (HTMLEventKeyList){0};
  g_html_event.wheel = (HTMLEventWheelList){0};

  g_html_event.listener_flags = 0;
}

unsigned int html_event_listener_flag(HTMLEventType type) { return 1u << type; }
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

  printf("[%d] HTML flags: %d : %d\n", type, g_html_event.listener_flags,
         has_listener);
  // return if event listener already set
  if (has_listener)
    return;

  // else set listener flag as active
  html_event_listener_flag_set(type);

  printf("set new listener callback\n");
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

  case HTMLEventType_Wheel:
    emscripten_set_wheel_callback(g_html_event.target, NULL, false,
                                  event_callback);
    break;

  default:
    break;
  }
}

int html_event_insert(HTMLEventVoid *event, void **entries, size_t *length,
                      size_t *capacity, size_t type_size,
                      HTMLEventType event_type, void *event_callback) {

  html_event_check_callback(event_type, event_callback);

  // check initialized
  if (*entries == NULL) {

    // allocate new list
    *capacity = HTML_EVENT_DEFAULT_CAPACITY;
    *length = 0;
    *entries = malloc(type_size * (*capacity));

    if (*entries == NULL) {
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

  // once we've checked if event list can store new event, we dynamically

  // retrieve entry pointer position
  HTMLEventVoid *cast_entry =
      (HTMLEventVoid *)((char *)(*entries) + (*length) * type_size);

  // append new event object to list
  memcpy(cast_entry, event, type_size);

  // allocate data on stack if any
  if (event->data != NULL) {

    html_event_data stored_data = malloc(event->size);

    if (stored_data != NULL) {
      cast_entry->data = stored_data;
      memcpy(cast_entry->data, event->data, event->size);
    } else {
      perror("Coudln't allocate memory for html event data.\n");
      return HTML_EVENT_ALLOC_FAIL;
    }
  }

  (*length)++;

  return HTML_EVENT_SUCCESS;
}

/**
   Add a mouse click event to the relative list.
 */
int html_event_add_mouse_down(HTMLEventMouse *event) {

  void *entries = &g_html_event.mouse_down.entries;
  size_t *length = &g_html_event.mouse_down.length;
  size_t *capacity = &g_html_event.mouse_down.capacity;
  size_t type_size = sizeof(HTMLEventMouse);
  HTMLEventType event_type = HTMLEventType_MouseMove;
  void *event_callback = html_event_listener_mouse_down;

  return html_event_insert(
      &(HTMLEventVoid){
          .callback = (void *)event->callback,
          .data = (void *)event->data,
          .size = event->size,
      },
      entries, length, capacity, type_size, event_type, event_callback);
}

/**
   Add a mouse move event to the relative list
 */
int html_event_add_mouse_move(HTMLEventMouse *event) {

  void *entries = &g_html_event.mouse_move.entries;
  size_t *length = &g_html_event.mouse_move.length;
  size_t *capacity = &g_html_event.mouse_move.capacity;
  size_t type_size = sizeof(HTMLEventMouse);
  HTMLEventType event_type = HTMLEventType_MouseMove;
  void *event_callback = html_event_listener_mouse_move;

  return html_event_insert(
      &(HTMLEventVoid){
          .callback = (void *)event->callback,
          .data = (void *)event->data,
          .size = event->size,
      },
      entries, length, capacity, type_size, event_type, event_callback);
}

/**
   Add a mouse wheel event to the relative list
 */
int html_event_add_wheel(HTMLEventWheel *event) {

  void *entries = &g_html_event.wheel.entries;
  size_t *length = &g_html_event.wheel.length;
  size_t *capacity = &g_html_event.wheel.capacity;
  size_t type_size = sizeof(HTMLEventWheel);
  HTMLEventType event_type = HTMLEventType_Wheel;
  void *event_callback = html_event_listener_wheel;

  return html_event_insert(
      &(HTMLEventVoid){
          .callback = (void *)event->callback,
          .data = (void *)event->data,
          .size = event->size,
      },
      entries, length, capacity, type_size, event_type, event_callback);
}

/**
   Add a key down  event to the relative list
 */
int html_event_add_key_down(HTMLEventKey *event) {

  void *entries = &g_html_event.key_down.entries;
  size_t *length = &g_html_event.key_down.length;
  size_t *capacity = &g_html_event.key_down.capacity;
  size_t type_size = sizeof(HTMLEventKey);
  HTMLEventType event_type = HTMLEventType_KeyDown;
  void *event_callback = html_event_listener_key_down;

  return html_event_insert(
      &(HTMLEventVoid){
          .callback = (void *)event->callback,
          .data = (void *)event->data,
          .size = event->size,
      },
      entries, length, capacity, type_size, event_type, event_callback);
}

/**
   Add a key up event to the relative list
 */
int html_event_add_key_up(HTMLEventKey *event) {

  void *entries = &g_html_event.key_up.entries;
  size_t *length = &g_html_event.key_up.length;
  size_t *capacity = &g_html_event.key_up.capacity;
  size_t type_size = sizeof(HTMLEventKey);
  HTMLEventType event_type = HTMLEventType_KeyUp;
  void *event_callback = html_event_listener_key_up;

  return html_event_insert(
      &(HTMLEventVoid){
          .callback = (void *)event->callback,
          .data = (void *)event->data,
          .size = event->size,
      },
      entries, length, capacity, type_size, event_type, event_callback);
}

/**

   ▗▖   ▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖▗▄▄▖  ▗▄▄▖
   ▐▌     █  ▐▌     █  ▐▌   ▐▛▚▖▐▌▐▌   ▐▌ ▐▌▐▌
   ▐▌     █   ▝▀▚▖  █  ▐▛▀▀▘▐▌ ▝▜▌▐▛▀▀▘▐▛▀▚▖ ▝▀▚▖
   ▐▙▄▄▖▗▄█▄▖▗▄▄▞▘  █  ▐▙▄▄▖▐▌  ▐▌▐▙▄▄▖▐▌ ▐▌▗▄▄▞▘

   Loop through each event list and executre callback

 */

bool html_event_listener_mouse_move(int eventType,
                                    const EmscriptenMouseEvent *mouseEvent,
                                    void *userData) {

  for (size_t i = 0; i < g_html_event.mouse_move.length; i++) {

    HTMLEventMouse *event = &g_html_event.mouse_move.entries[i];
    em_mouse_callback_func callback = event->callback;
    void *data = event->data;
    // pass down the parent arguments, exepct the userData get replaced by
    // callback data
    callback(eventType, mouseEvent, data);
  }

  return EM_FALSE;
}

bool html_event_listener_mouse_down(int eventType,
                                    const EmscriptenMouseEvent *mouseEvent,
                                    void *userData) {

  for (size_t i = 0; i < g_html_event.mouse_down.length; i++) {

    HTMLEventMouse *event = &g_html_event.mouse_down.entries[i];
    em_mouse_callback_func callback = event->callback;
    void *data = event->data;
    // pass down the parent arguments, exepct the userData get replaced by
    // callback data
    callback(eventType, mouseEvent, data);
  }

  return EM_FALSE;
}

bool html_event_listener_wheel(int eventType,
                               const EmscriptenWheelEvent *wheelEvent,
                               void *userData) {

  for (size_t i = 0; i < g_html_event.wheel.length; i++) {

    HTMLEventWheel *event = &g_html_event.wheel.entries[i];
    em_wheel_callback_func callback = event->callback;
    void *data = event->data;
    // pass down the parent arguments, exepct the userData get replaced by
    // callback data
    callback(eventType, wheelEvent, data);
  }

  return EM_TRUE;
}

bool html_event_listener_key_up(int eventType,
                                const EmscriptenKeyboardEvent *keyboardEvent,
                                void *userData) {

  for (size_t i = 0; i < g_html_event.wheel.length; i++) {

    HTMLEventKey *event = &g_html_event.key_up.entries[i];
    em_key_callback_func callback = event->callback;
    void *data = event->data;
    // pass down the parent arguments, exepct the userData get replaced by
    // callback data
    callback(eventType, keyboardEvent, data);
  }

  return EM_FALSE;
}

bool html_event_listener_key_down(int eventType,
                                  const EmscriptenKeyboardEvent *keyboardEvent,
                                  void *userData) {

  for (size_t i = 0; i < g_html_event.wheel.length; i++) {

    HTMLEventKey *event = &g_html_event.key_down.entries[i];
    em_key_callback_func callback = event->callback;
    void *data = event->data;
    // pass down the parent arguments, exepct the userData get replaced by
    // callback data
    callback(eventType, keyboardEvent, data);
  }

  return EM_FALSE;
}

void html_event_lock_mouse() {
  emscripten_request_pointerlock(g_html_event.target, true);
}
