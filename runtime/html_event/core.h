#ifndef _HTML_EVENT_CORE_H_
#define _HTML_EVENT_CORE_H_
#include "emscripten/em_types.h"
#include "emscripten/html5.h"

#define HTML_EVENT_SUCCESS 0
#define HTML_EVENT_ALLOC_FAIL 1
#define HTML_EVENT_UNDEF_ERROR 2
#define HTML_EVENT_DEFAULT_CAPACITY 64

typedef const char *html_event_target;
typedef EMSCRIPTEN_RESULT (*emscripten_event_listener)(const char *, void *,
                                                       bool,
                                                       em_mouse_callback_func);

typedef enum {
  HTMLEventType_MouseMove,
  HTMLEventType_Click,
  HTMLEventType_KeyDown,
  HTMLEventType_KeyUp,
  HTMLEventType_Wheel,
  HTMLEventType_Void,
} HTMLEventType;

// anonymous event
typedef struct {
  void *callback;
  void *data;
} HTMLEventVoid;

// mouse events
typedef struct {
  em_mouse_callback_func callback;
  void *data;
} HTMLEventMouse;

typedef struct {
  HTMLEventMouse *entries;
  size_t length;
  size_t capacity;
} HTMLEventMouseList;

// wheel events
typedef struct {
  em_wheel_callback_func callback;
  void *data;
} HTMLEventWheel;

typedef struct {
  HTMLEventWheel *entries;
  size_t length;
  size_t capacity;
} HTMLEventWheelList;

// key events
typedef struct {
  em_key_callback_func callback;
  void *data;
} HTMLEventKey;

typedef struct {
  HTMLEventKey *entries;
  size_t length;
  size_t capacity;
} HTMLEventKeyList;

typedef struct {

  html_event_target target;

  HTMLEventKeyList key;
  HTMLEventWheelList wheel;
  HTMLEventMouseList mouse;

} HTMLEvent;

extern HTMLEvent g_html_event;

void html_event_init(html_event_target);

// mouse events
int html_event_add_mouse_click(HTMLEventMouse *);
int html_event_add_mouse_move(HTMLEventMouse *);

// wheel events
int html_event_add_wheel(HTMLEventWheel *);

// key events
int html_event_add_key_down(HTMLEventKey *);
int html_event_add_key_up(HTMLEventKey *);

#endif
