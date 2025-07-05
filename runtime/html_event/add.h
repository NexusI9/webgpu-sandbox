#ifndef _EVENT_LISTENER_ADD_H_
#define _EVENT_LISTENER_ADD_H_


#include "core.h"

// mouse events
int html_event_add_mouse_down(HTMLEventMouse *);
int html_event_add_mouse_move(HTMLEventMouse *);

// wheel events
int html_event_add_wheel(HTMLEventWheel *);

// key events
int html_event_add_key_down(HTMLEventKey *);
int html_event_add_key_up(HTMLEventKey *);


#endif
