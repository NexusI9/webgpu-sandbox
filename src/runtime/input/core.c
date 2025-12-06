#include "core.h"

#include <emscripten/em_types.h>
#include <emscripten/html5.h>
#include <stdio.h>
#include <string.h>

#include "backend/registry.h"
#include "keyrecord.h"
#include "runtime/html_event/add.h"
#include "runtime/html_event/core.h"
#include "utils/math.h"

Input g_input = {0};

bool input_callback_key_down(int eventType,
                             const EmscriptenKeyboardEvent *keyEvent,
                             void *userData) {

  unsigned int keyCode = keyEvent->keyCode;
  if (keyCode < INPUT_KEY_LENGTH && g_input.keys[keyCode] == false)
    g_input.keys[keyCode] = true;

  return false;
}

bool input_callback_key_up(int eventType,
                           const EmscriptenKeyboardEvent *keyEvent,
                           void *userData) {

  unsigned int keyCode = keyEvent->keyCode;
  if (keyCode < INPUT_KEY_LENGTH && g_input.keys[keyCode] == true)
    g_input.keys[keyCode] = false;

  return false;
}

bool input_callback_mouse_move(int eventType,
                               const EmscriptenMouseEvent *mouseEvent,
                               void *userData) {

  // movement
  g_input.mouse.movement.x = MIN(mouseEvent->movementX, INPUT_MAX_MOVEMENT);
  g_input.mouse.movement.y = MIN(mouseEvent->movementY, INPUT_MAX_MOVEMENT);

  // position (use movement cause of pointer lock)
  g_input.mouse.delta.x += mouseEvent->movementX;
  g_input.mouse.delta.y += mouseEvent->movementY;

  g_input.mouse.x = mouseEvent->clientX;
  g_input.mouse.y = mouseEvent->clientY;
  return false;
}

bool input_callback_mouse_down(int eventType,
                               const EmscriptenMouseEvent *mouseEvent,
                               void *userData) {
  g_input.mouse.state[mouseEvent->button] = InputMouseState_Down;
  return EM_FALSE;
}

bool input_callback_mouse_up(int eventType,
                             const EmscriptenMouseEvent *mouseEvent,
                             void *usetData) {
  g_input.mouse.state[mouseEvent->button] = InputMouseState_Up;
  return EM_FALSE;
}

bool input_callback_wheel(int eventType, const EmscriptenWheelEvent *wheelEvent,
                          void *userData) {

  g_input.mouse.wheel.deltaX = wheelEvent->deltaX;
  g_input.mouse.wheel.deltaY = wheelEvent->deltaY;

  g_input.mouse.pan.x += wheelEvent->deltaX;
  g_input.mouse.pan.y += wheelEvent->deltaY;

  // zoom
  if (input_key(INPUT_KEY_LEFTWINDOW))
    g_input.mouse.zoom += wheelEvent->deltaY;

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
void input_init(const InputDescriptor *desc) {

  g_input.mouse.sensitivity = desc->mouse_sensitivity;
  g_input.mouse.wheel.sensitivity = desc->wheel_sensitivity;

  // key down/up event listener
  html_event_add_key_down(&(HTMLEventKey){
      .callback = input_callback_key_down,
      .data = NULL,
      .size = 0,
  });

  html_event_add_key_up(&(HTMLEventKey){
      .callback = input_callback_key_up,
      .data = NULL,
      .size = 0,
  });

  // mouse move event listener
  html_event_add_mouse_move(&(HTMLEventMouse){
      .callback = input_callback_mouse_move,
      .data = NULL,
      .size = 0,
  });

  // mouse down event listener
  html_event_add_mouse_down(&(HTMLEventMouse){
      .callback = input_callback_mouse_down,
      .data = NULL,
      .size = 0,
  });

  // mouse move event listener
  html_event_add_mouse_up(&(HTMLEventMouse){
      .callback = input_callback_mouse_up,
      .data = NULL,
      .size = 0,
  });

  // scroll event listener
  // emscripten_set_wheel_callback(target, NULL, false, input_wheel);
  html_event_add_wheel(&(HTMLEventWheel){
      .callback = input_callback_wheel,
      .data = NULL,
      .size = 0,
  });

  // create key sequence recorder
  keyrec_sequence_listener_create(&g_input.sequence_listener,
                                  INPUT_KEY_RECORD_CAPACITY);
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

void input_mouse_NDC(const float x, const float y, const int width,
                     const int height, float *dest_x, float *dest_y) {
  *dest_x = 2.0f * x / width - 1.0f;
  *dest_y = 1.0f - 2.0f * y / height;
}

/**
   Below key record function uses the g_input, hence we don't put them in the
   keyrec files since they use the global variable. They serve as alias by
   directly passing the g_input.record_listener as argument.
 */
KeyRecordStatus input_key_sequence_add(KeyRecordSequence *seq) {
  return keyrec_add_sequence(&g_input.sequence_listener, seq);
}

KeyRecordStatus input_key_sequence_destroy_by_id(reg_id_t id) {

  // seach all sequence with the owner id
  KeyRecordSequenceListResult result =
      keyrec_find_sequence_by_id(&g_input.sequence_listener, id);

  // destroy/ free them
  for (size_t i = 0; i < result.length; i++)
    keyrec_destroy_sequence(&g_input.sequence_listener,
                            &g_input.sequence_listener.entries[i]);

  return KeyRecordStatus_Success;
}
