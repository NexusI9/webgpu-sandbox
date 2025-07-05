#ifndef _HTML_EVENT_REMOVE_H_
#define _HTML_EVENT_REMOVE_H_

#include "core.h"

// mouse events
void html_event_remove_mouse_down(id_t);
void html_event_remove_mouse_move(id_t);

// wheel events
void html_event_remove_wheel(id_t);

// key events
void html_event_remove_key_down(id_t);
void html_event_remove_key_up(id_t);

#endif
