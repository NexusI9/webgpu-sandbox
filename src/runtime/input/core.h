#ifndef _INPUT_CORE_H_
#define _INPUT_CORE_H_

#include <stdbool.h>
#include <stdint.h>

#include "./keyrecord.h"
#include "backend/registry.h"
#include "emscripten/html5.h"
#include "utils/defines.h"

static const int INPUT_KEY_FORWARD_FR = 90;
static const int INPUT_KEY_BACKWARD_FR = 83;
static const int INPUT_KEY_LEFT_FR = 81;
static const int INPUT_KEY_RIGHT_FR = 68;
static const int INPUT_KEY_CAP = 16;
static const int INPUT_KEY_ALT = 18;
static const int INPUT_KEY_CMD = 91;
static const int INPUT_KEY_CTRL = 17;
static const int INPUT_KEY_SPACE = 32;

static const int INPUT_KEY_LENGTH = 128;
static const int INPUT_MAX_MOVEMENT = 20;

#define INPUT_EVENT_DEFAULT_TARGET "body"

typedef enum {
  InputMouseButton_Left,
  InputMouseButton_Middle,
  InputMouseButton_Right,
  InputMouseButton_COUNT,
} InputMouseButton;

typedef enum {
  InputMouseState_Up,
  InputMouseState_Down,
} InputMouseState;

typedef enum {
  InputLockState_Unlocked = 0,
  InputLockState_Keyboard = 1 << 0,
  InputLockState_Mouse = 1 << 1,
} InputLockState;

typedef struct {

  int locked; // need to look when focused on ui
  bool keys[INPUT_KEY_LENGTH];
  KeyRecordSequenceList sequence_listener;

  struct {
    int x, y;
    InputMouseState state[InputMouseButton_COUNT];
    float sensitivity;

    struct {
      int x, y;
    } delta;

    struct {
      int x, y;
    } movement;

    struct {
      double deltaX, deltaY;
      float sensitivity;
    } wheel;

    struct {
      int x, y;
    } pan;

    float zoom;

  } mouse;

} Input;

extern Input g_input;

typedef struct {
  float mouse_sensitivity;
  float wheel_sensitivity;
} InputDescriptor;

EXTERN_C_BEGIN

void input_set_key(unsigned int, bool);
void input_disable_all_keys();

void input_init(const InputDescriptor *);

bool input_key(unsigned int);

void input_wheel_reset();

// HTML Callbacks
bool input_callback_key_down(int, const EmscriptenKeyboardEvent *, void *);
bool input_callback_key_up(int, const EmscriptenKeyboardEvent *, void *);
bool input_callback_mouse_move(int, const EmscriptenMouseEvent *, void *);
bool input_callback_mouse_down(int, const EmscriptenMouseEvent *, void *);
bool input_callback_mouse_up(int, const EmscriptenMouseEvent *, void *);
bool input_callback_wheel(int, const EmscriptenWheelEvent *, void *);

// Accessors
static inline float input_pan_x() { return g_input.mouse.pan.x; }
static inline float input_pan_y() { return g_input.mouse.pan.y; }
static inline void input_pan(ivec2 dest) {
  glm_ivec2_copy((ivec2){g_input.mouse.pan.x, g_input.mouse.pan.y}, dest);
}

static inline float input_wheel_x() {
  return (float)g_input.mouse.wheel.deltaX;
}
static inline float input_wheel_y() {
  return (float)g_input.mouse.wheel.deltaY;
}
static inline void input_wheel(vec2 dest) {
  glm_vec2_copy((vec2){(float)g_input.mouse.wheel.deltaX,
                       (float)g_input.mouse.wheel.deltaY},
                dest);
}

static inline int input_mouse_x() { return g_input.mouse.x; }
static inline int input_mouse_y() { return g_input.mouse.y; }
static inline void input_mouse(ivec2 dest) {
  glm_ivec2_copy((ivec2){g_input.mouse.x, g_input.mouse.y}, dest);
}

static inline float input_zoom() { return g_input.mouse.zoom; }

// Keyboard Sequences
KeyRecordStatus input_key_sequence_add(KeyRecordSequence *);
KeyRecordStatus input_key_sequence_destroy_by_id(reg_id_t);

// Utils
void input_mouse_NDC(const float, const float, const int, const int, float *,
                     float *);

EXTERN_C_END
#endif
