#ifndef _INPUT_CORE_H_
#define _INPUT_CORE_H_

#include "./keyrecord.h"
#include "emscripten/html5.h"
#include <stdbool.h>
#include <stdint.h>

#define INPUT_KEY_FORWARD_FR 90
#define INPUT_KEY_BACKWARD_FR 83
#define INPUT_KEY_LEFT_FR 81
#define INPUT_KEY_RIGHT_FR 68
#define INPUT_KEY_CAP 16
#define INPUT_KEY_ALT 18
#define INPUT_KEY_CMD 91
#define INPUT_KEY_CTRL 17
#define INPUT_KEY_SPACE 32

#define INPUT_EVENT_DEFAULT_TARGET "body"
#define INPUT_KEY_LENGTH 128
#define INPUT_MAX_MOVEMENT 20
#define INPUT_MOUSE_SENSITIVITY 0.02f
#define INPUT_WHEEL_SENSITIVITY 0.02f

typedef enum {
  InputMouseState_Up,
  InputMouseState_Down,
} InputMouseState;

typedef struct {

  bool keys[INPUT_KEY_LENGTH];
  KeyRecordSequenceList sequence_listener;

  struct {

    int x, y;
    InputMouseState state;

    struct {
      int x, y;
    } delta;

    struct {
      int x, y;
    } movement;

    struct {
      double deltaX, deltaY;
    } wheel;

  } mouse;

} Input;

extern Input g_input;

void input_set_key(unsigned int, bool);
void input_disable_all_keys();

void input_listen();

bool input_key(unsigned int);

void input_wheel_reset();

bool input_key_down(int, const EmscriptenKeyboardEvent *, void *);
bool input_key_up(int, const EmscriptenKeyboardEvent *, void *);
bool input_mouse_move(int, const EmscriptenMouseEvent *, void *);
bool input_mouse_down(int, const EmscriptenMouseEvent *, void *);
bool input_mouse_up(int, const EmscriptenMouseEvent *, void *);
bool input_wheel(int, const EmscriptenWheelEvent *, void *);

void input_mouse_NDC(const float, const float, const int, const int, float *,
                     float *);

KeyRecordStatus input_key_sequence_add(KeyRecordSequence *);
KeyRecordStatus input_key_sequence_destroy_by_id(reg_id_t);
#endif
