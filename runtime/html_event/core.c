#include "core.h"
#include "emscripten/em_asm.h"
#include <string.h>

/**

  ▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖▗▄▄▄▖
    █  ▐▛▚▖▐▌  █    █
    █  ▐▌ ▝▜▌  █    █
  ▗▄█▄▖▐▌  ▐▌▗▄█▄▖  █

 */

HTMLEvent g_html_event = {0};

static inline void html_event_prevent_context_menu();

/**
   Disable context menu on canvas (useful for mouse events).
   Emscripten doesn't set a context menu callback yet, thus we use EM_ASM
   directly inject the javascript code.
   However can only pass int or double as argument. Need to pass the target
   pointer, convert the pointer to string with UTF8ToString(ptr).
 */
void html_event_prevent_context_menu() {

#ifdef __EMSCRIPTEN__
  EM_ASM(
      {
        let idPtr = $0;
        let idString = UTF8ToString(idPtr);
        let canvas = document.getElementById(idString);
        if (canvas)
          canvas.addEventListener(
              'contextmenu', function(e) { e.preventDefault(); });
      },
      g_html_event.target);
#endif
}

void html_event_init(html_event_target target) {
  g_html_event.target = strdup(target);
  g_html_event.listener_flags = 0;

  // reset lists
  g_html_event.mouse_move = (HTMLEventMouseList){0};
  g_html_event.mouse_down = (HTMLEventMouseList){0};
  g_html_event.key_down = (HTMLEventKeyList){0};
  g_html_event.key_up = (HTMLEventKeyList){0};
  g_html_event.wheel = (HTMLEventWheelList){0};

  // disable browser contextmenu
  html_event_prevent_context_menu();
}

void html_event_lock_mouse() {
  emscripten_request_pointerlock(g_html_event.target, true);
}
