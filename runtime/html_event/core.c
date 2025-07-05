#include "core.h"
#include <string.h>


/**

  ▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖▗▄▄▄▖
    █  ▐▛▚▖▐▌  █    █
    █  ▐▌ ▝▜▌  █    █
  ▗▄█▄▖▐▌  ▐▌▗▄█▄▖  █

 */

HTMLEvent g_html_event = {0};

void html_event_init(html_event_target target) {
  g_html_event.target = strdup(target);
  g_html_event.listener_flags = 0;

  // reset lists
  g_html_event.mouse_move = (HTMLEventMouseList){0};
  g_html_event.mouse_down = (HTMLEventMouseList){0};
  g_html_event.key_down = (HTMLEventKeyList){0};
  g_html_event.key_up = (HTMLEventKeyList){0};
  g_html_event.wheel = (HTMLEventWheelList){0};
}


void html_event_lock_mouse() {
  emscripten_request_pointerlock(g_html_event.target, true);
}
