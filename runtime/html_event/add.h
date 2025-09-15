#ifndef _EVENT_LISTENER_ADD_H_
#define _EVENT_LISTENER_ADD_H_

#include "core.h"

typedef HTMLEventStatus (*html_event_mouse)(HTMLEventMouse *);
typedef HTMLEventStatus (*html_event_wheel)(HTMLEventWheel *);
typedef HTMLEventStatus (*html_event_key)(HTMLEventKey *);

// mouse events
HTMLEventStatus html_event_add_mouse_down(HTMLEventMouse *);
HTMLEventStatus html_event_add_mouse_up(HTMLEventMouse *);
HTMLEventStatus html_event_add_mouse_move(HTMLEventMouse *);

// wheel events
HTMLEventStatus html_event_add_wheel(HTMLEventWheel *);

// key events
HTMLEventStatus html_event_add_key_down(HTMLEventKey *);
HTMLEventStatus html_event_add_key_up(HTMLEventKey *);

#endif
