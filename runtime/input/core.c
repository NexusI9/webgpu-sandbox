#include "core.h"
#include "../html_event/html_event.h"
#include "../utils/math.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

Input g_input = {0};

bool input_keyboard(int eventType, const EmscriptenKeyboardEvent *keyEvent,
                    void *userData) {

  unsigned int keyCode = keyEvent->keyCode;
  switch (eventType) {

  case EMSCRIPTEN_EVENT_KEYDOWN:
    if (keyCode < INPUT_KEY_LENGTH && g_input.keys[keyCode] == false)
      g_input.keys[keyCode] = true;
    break;

  case EMSCRIPTEN_EVENT_KEYUP:
    if (keyCode < INPUT_KEY_LENGTH && g_input.keys[keyCode] == true)
      g_input.keys[keyCode] = false;
    break;
  }

  return false;
}

bool input_mouse_move(int eventType, const EmscriptenMouseEvent *mouseEvent,
                      void *userData) {

  // movement
  g_input.mouse.movement.x = MIN(mouseEvent->movementX, INPUT_MAX_MOVEMENT);
  g_input.mouse.movement.y = MIN(mouseEvent->movementY, INPUT_MAX_MOVEMENT);

  // position (use movement cause of pointer lock)
  g_input.mouse.x += mouseEvent->movementX;
  g_input.mouse.y += mouseEvent->movementY;

  return false;
}

bool input_wheel(int eventType, const EmscriptenWheelEvent *wheelEvent,
                 void *userData) {

  g_input.mouse.wheel.deltaX = wheelEvent->deltaX;
  g_input.mouse.wheel.deltaY = wheelEvent->deltaY;

  // returning true call preventDefault
  return EM_TRUE;
}

void input_set_key(unsigned int key, bool state) { g_input.keys[key] = state; }

void input_disable_all_keys() { memset(g_input.keys, 0, sizeof(g_input.keys)); }

/**
   Add events listeners.
   Update input global attributes. Useful to retrieves
   those attributes during the draw.
 */
void input_listen() {

  // key down/up event listener
  html_event_add_key_down(&(HTMLEventKey){
      .callback = input_keyboard,
      .data = NULL,
      .size = 0,
  }); 

  html_event_add_key_up(&(HTMLEventKey){
      .callback = input_keyboard,
      .data = NULL,
      .size = 0,
  });

  // mouse move event listener
  html_event_add_mouse_move(&(HTMLEventMouse){
      .callback = input_mouse_move,
      .data = NULL,
      .size = 0,
  });

  // scroll event listener
  // emscripten_set_wheel_callback(target, NULL, false, input_wheel);
  html_event_add_wheel(&(HTMLEventWheel){
      .callback = input_wheel,
      .data = NULL,
      .size = 0,
  });
}

bool input_key(unsigned int key) {

  if (key < INPUT_KEY_LENGTH)
    return g_input.keys[key];

  return false;
}

/**
   Reset input wheel due to delta lingering
 */
void input_wheel_reset() {
  g_input.mouse.wheel.deltaX = 0.0f;
  g_input.mouse.wheel.deltaY = 0.0f;
}
