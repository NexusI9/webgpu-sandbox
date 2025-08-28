#ifndef _SCENE_EVENT_CAMERA_H_
#define _SCENE_EVENT_CAMERA_H_

#include "../core.h"
#include "emscripten/html5.h"

void scene_event_html(Scene *);

bool scene_event_html_mouse(int, const EmscriptenMouseEvent *, void *);
bool scene_event_html_wheel(int, const EmscriptenWheelEvent *, void *);
bool scene_event_html_key(int, const EmscriptenKeyboardEvent *, void *);

#endif
