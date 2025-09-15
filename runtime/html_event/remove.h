#ifndef _HTML_EVENT_REMOVE_H_
#define _HTML_EVENT_REMOVE_H_

#include "core.h"

// mouse events
void html_event_remove_mouse_down(reg_id_t);
void html_event_remove_mouse_up(reg_id_t);
void html_event_remove_mouse_move(reg_id_t);

// wheel events
void html_event_remove_wheel(reg_id_t);

// key events
void html_event_remove_key_down(reg_id_t);
void html_event_remove_key_up(reg_id_t);

#endif
