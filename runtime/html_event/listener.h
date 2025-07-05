#ifndef _HTML_EVENT_LISTENER_H_
#define _HTML_EVENT_LISTENER_H_

#include <emscripten/html5.h>

// mouse listeners
bool html_event_listener_mouse_move(int, const EmscriptenMouseEvent *, void *);
bool html_event_listener_mouse_down(int, const EmscriptenMouseEvent *, void *);

// wheel listeners
bool html_event_listener_wheel(int, const EmscriptenWheelEvent *, void *);

// key listeners
bool html_event_listener_key_down(int, const EmscriptenKeyboardEvent *, void *);
bool html_event_listener_key_up(int, const EmscriptenKeyboardEvent *, void *);

#endif
