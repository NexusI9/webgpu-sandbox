#ifndef _INPUT_EVENT_H_
#define _INPUT_EVENT_H_

#include "emscripten/html5.h"

void input_on_mouse_move(const char *, em_mouse_callback_func, void *);
void input_on_mouse_down(const char *, em_mouse_callback_func, void *);

#endif
