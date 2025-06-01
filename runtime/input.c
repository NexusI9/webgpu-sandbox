#include "input.h"
#include "../utils/math.h"
#include "emscripten/html5.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

Input g_input = {0};

static bool input_key_down(int eventType,
                           const EmscriptenKeyboardEvent *keyEvent,
                           void *userData) {

  unsigned int keyCode = keyEvent->keyCode;
  if (keyCode < INPUT_KEY_LENGTH && g_input.keys[keyCode] == false)
    g_input.keys[keyCode] = true;

  return false;
}

static bool input_key_up(int eventType, const EmscriptenKeyboardEvent *keyEvent,
                         void *userData) {

  unsigned int keyCode = keyEvent->keyCode;
  if (keyCode < INPUT_KEY_LENGTH && g_input.keys[keyCode] == true)
    g_input.keys[keyCode] = false;

  return false;
}

static bool input_mouse_move(int eventType,
                             const EmscriptenMouseEvent *mouseEvent,
                             void *userData) {

  // movement
  g_input.mouse.movement.x = MIN(mouseEvent->movementX, INPUT_MAX_MOVEMENT);
  g_input.mouse.movement.y = MIN(mouseEvent->movementY, INPUT_MAX_MOVEMENT);

  // position (use movement cause of pointer lock)
  g_input.mouse.x += mouseEvent->movementX;
  g_input.mouse.y += mouseEvent->movementY;

  return false;
}

static bool input_wheel(int eventType, const EmscriptenWheelEvent *wheelEvent,
                        void *userData) {

  g_input.mouse.wheel.deltaX = wheelEvent->deltaX;
  g_input.mouse.wheel.deltaY = wheelEvent->deltaY;

  return false;
}

void input_set_key(unsigned int key, bool state) { g_input.keys[key] = state; }

void input_disable_all_keys() { memset(g_input.keys, 0, sizeof(g_input.keys)); }

void input_listen() {

  const char *target =
      EMSCRIPTEN_EVENT_TARGET_DOCUMENT; // EVENT_DEFAULT_TARGET;
  // key down event listener
  emscripten_set_keydown_callback(target, NULL, false, input_key_down);

  // key up event listener
  emscripten_set_keyup_callback(target, NULL, false, input_key_up);

  // mouse move event listener
  emscripten_set_mousemove_callback(target, NULL, false, input_mouse_move);

  // scroll event listener
  emscripten_set_wheel_callback(target, NULL, false, input_wheel);
}

bool input_key(unsigned int key) {

  if (key < INPUT_KEY_LENGTH)
    return g_input.keys[key];

  return false;
}
