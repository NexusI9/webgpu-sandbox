#ifndef _HTML_EVENT_CORE_H_
#define _HTML_EVENT_CORE_H_
#include "../backend/registry.h"
#include "emscripten/em_types.h"
#include "emscripten/html5.h"
#include <stdint.h>

#define HTML_EVENT_SUCCESS 0
#define HTML_EVENT_ALLOC_FAIL 1
#define HTML_EVENT_UNDEF_ERROR 2
#define HTML_EVENT_DEFAULT_CAPACITY 64

typedef const char *html_event_target;
typedef void *html_event_data;
typedef bool (*emscripten_event_listener)(const char *, void *, bool,
                                          em_mouse_callback_func);
typedef bool (*html_event_destructor_callback)(void *);

typedef enum {
  HTMLEventType_MouseMove,
  HTMLEventType_MouseDown,
  HTMLEventType_KeyDown,
  HTMLEventType_KeyUp,
  HTMLEventType_Wheel
} HTMLEventType;

// anonymous event
typedef struct {
  void *callback;
  void *destructor;
  html_event_data data;
  size_t size;
  id_t owner;
} HTMLEventVoid;

// mouse events
typedef struct {
  em_mouse_callback_func callback;
  html_event_destructor_callback destructor;
  html_event_data data;
  size_t size;
  id_t owner;
} HTMLEventMouse;

typedef struct {
  HTMLEventMouse *entries;
  size_t length;
  size_t capacity;
} HTMLEventMouseList;

// wheel events
typedef struct {
  em_wheel_callback_func callback;
  html_event_destructor_callback destructor;
  html_event_data data;
  size_t size;
  id_t owner;
} HTMLEventWheel;

typedef struct {
  HTMLEventWheel *entries;
  size_t length;
  size_t capacity;
} HTMLEventWheelList;

// key events
typedef struct {
  em_key_callback_func callback;
  html_event_destructor_callback destructor;
  html_event_data data;
  size_t size;
  id_t owner;
} HTMLEventKey;

typedef struct {
  HTMLEventKey *entries;
  size_t length;
  size_t capacity;
} HTMLEventKeyList;

typedef struct {

  html_event_target target;
  uint8_t listener_flags;

  // mouse
  HTMLEventMouseList mouse_move;
  HTMLEventMouseList mouse_down;

  // key
  HTMLEventKeyList key_up;
  HTMLEventKeyList key_down;

  // wheel
  HTMLEventWheelList wheel;

} HTMLEvent;

extern HTMLEvent g_html_event;

void html_event_init(html_event_target);
void html_event_lock_mouse();

#endif
