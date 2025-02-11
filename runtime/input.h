#ifndef _INPUT_H_
#define _INPUT_H_

#include <stdbool.h>

#define INPUT_KEY_LENGTH 128

typedef struct {

  bool keys[INPUT_KEY_LENGTH];

} input;

extern input g_input;

void input_set_key(unsigned int, bool);
void input_disable_all_keys();

void input_listen();

bool input_key(unsigned int);

#endif
