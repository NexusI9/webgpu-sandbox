#ifndef _INPUT_CORE_H_
#define _INPUT_CORE_H_

#include <stdbool.h>
#include <stdint.h>

#include "./keyrecord.h"
#include "backend/registry.h"
#include "emscripten/html5.h"
#include "utils/defines.h"

typedef enum InputKey {
  INPUT_KEY_0 = 48,
  INPUT_KEY_1 = 49,
  INPUT_KEY_2 = 50,
  INPUT_KEY_3 = 51,
  INPUT_KEY_4 = 52,
  INPUT_KEY_5 = 53,
  INPUT_KEY_6 = 54,
  INPUT_KEY_7 = 55,
  INPUT_KEY_8 = 56,
  INPUT_KEY_9 = 57,

  INPUT_KEY_BACKSPACE = 8,
  INPUT_KEY_TAB = 9,
  INPUT_KEY_ENTER = 13,
  INPUT_KEY_SHIFT = 16,
  INPUT_KEY_CTRL = 17,
  INPUT_KEY_ALT = 18,
  INPUT_KEY_PAUSEBREAK = 19,
  INPUT_KEY_CAPSLOCK = 20,
  INPUT_KEY_ESC = 27,
  INPUT_KEY_SPACE = 32,
  INPUT_KEY_PAGEUP = 33,
  INPUT_KEY_PAGEDOWN = 34,
  INPUT_KEY_END = 35,
  INPUT_KEY_HOME = 36,
  INPUT_KEY_LEFTARROW = 37,
  INPUT_KEY_UPARROW = 38,
  INPUT_KEY_RIGHTARROW = 39,
  INPUT_KEY_DOWNARROW = 40,
  INPUT_KEY_PRINT_SCREEN = 44,
  INPUT_KEY_INSERT = 45,
  INPUT_KEY_DELETE = 46,

  INPUT_KEY_A = 65,
  INPUT_KEY_B = 66,
  INPUT_KEY_C = 67,
  INPUT_KEY_D = 68,
  INPUT_KEY_E = 69,
  INPUT_KEY_F = 70,
  INPUT_KEY_G = 71,
  INPUT_KEY_H = 72,
  INPUT_KEY_I = 73,
  INPUT_KEY_J = 74,
  INPUT_KEY_K = 75,
  INPUT_KEY_L = 76,
  INPUT_KEY_M = 77,
  INPUT_KEY_N = 78,
  INPUT_KEY_O = 79,
  INPUT_KEY_P = 80,
  INPUT_KEY_Q = 81,
  INPUT_KEY_R = 82,
  INPUT_KEY_S = 83,
  INPUT_KEY_T = 84,
  INPUT_KEY_U = 85,
  INPUT_KEY_V = 86,
  INPUT_KEY_W = 87,
  INPUT_KEY_X = 88,
  INPUT_KEY_Y = 89,
  INPUT_KEY_Z = 90,

  INPUT_KEY_LEFTWINDOW = 91,
  INPUT_KEY_RIGHTWINDOW = 92,
  INPUT_KEY_SELECT = 93,

  INPUT_KEY_NUMPAD0 = 96,
  INPUT_KEY_NUMPAD1 = 97,
  INPUT_KEY_NUMPAD2 = 98,
  INPUT_KEY_NUMPAD3 = 99,
  INPUT_KEY_NUMPAD4 = 100,
  INPUT_KEY_NUMPAD5 = 101,
  INPUT_KEY_NUMPAD6 = 102,
  INPUT_KEY_NUMPAD7 = 103,
  INPUT_KEY_NUMPAD8 = 104,
  INPUT_KEY_NUMPAD9 = 105,

  INPUT_KEY_MULTIPLY = 106,
  INPUT_KEY_ADD = 107,
  INPUT_KEY_SUBTRACT = 109,
  INPUT_KEY_DECIMAL = 110,
  INPUT_KEY_DIVIDE = 111,

  INPUT_KEY_F1 = 112,
  INPUT_KEY_F2 = 113,
  INPUT_KEY_F3 = 114,
  INPUT_KEY_F4 = 115,
  INPUT_KEY_F5 = 116,
  INPUT_KEY_F6 = 117,
  INPUT_KEY_F7 = 118,
  INPUT_KEY_F8 = 119,
  INPUT_KEY_F9 = 120,
  INPUT_KEY_F10 = 121,
  INPUT_KEY_F11 = 122,
  INPUT_KEY_F12 = 123,

  INPUT_KEY_NUMLOCK = 144,
  INPUT_KEY_SCROLLLOCK = 145,

  INPUT_KEY_SEMICOLON = 186,
  INPUT_KEY_EQUALSIGN = 187,
  INPUT_KEY_COMMA = 188,
  INPUT_KEY_DASH = 189,
  INPUT_KEY_PERIOD = 190,
  INPUT_KEY_SLASH = 191,
  INPUT_KEY_GRAVE = 192,
  INPUT_KEY_OPENBRACKET = 219,
  INPUT_KEY_BACKSLASH = 220,
  INPUT_KEY_CLOSEBRACKET = 221,
  INPUT_KEY_SINGLEQUOTE = 222
} InputKey;

static const int INPUT_KEY_COUNT = 128;
static const int INPUT_MAX_MOVEMENT = 20;
static const char *INPUT_EVENT_DEFAULT_TARGET = "body";

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
  bool keys[INPUT_KEY_COUNT];
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

static inline void input_set_mouse_sensitivity(const float value) {
  g_input.mouse.sensitivity = value;
}

static inline void input_set_wheel_sensitivity(const float value) {
  g_input.mouse.wheel.sensitivity = value;
}

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

static inline float input_wheel_sensitivity() {
  return (float)g_input.mouse.wheel.sensitivity;
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
