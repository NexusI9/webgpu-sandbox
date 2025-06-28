#include "event.h"

void input_on_mouse_move(const char *target, em_mouse_callback_func callback,
                         void *arg) {

  emscripten_set_mousemove_callback(target, arg, false, callback);
}

void input_on_mouse_down(const char *target, em_mouse_callback_func callback,
                         void *arg) {
  // mouse move event listener
  emscripten_set_mousedown_callback(target, arg, false, callback);
}
