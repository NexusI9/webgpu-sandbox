#ifndef _INPUT_H_
#define _INPUT_H_

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

typedef struct {

  bool keys[INPUT_KEY_LENGTH];

  struct {

    int x, y;

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

#endif
