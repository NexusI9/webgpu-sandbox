#include "listener.h"
#include "core.h"

/**

   ▗▖   ▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖▗▄▄▖  ▗▄▄▖
   ▐▌     █  ▐▌     █  ▐▌   ▐▛▚▖▐▌▐▌   ▐▌ ▐▌▐▌
   ▐▌     █   ▝▀▚▖  █  ▐▛▀▀▘▐▌ ▝▜▌▐▛▀▀▘▐▛▀▚▖ ▝▀▚▖
   ▐▙▄▄▖▗▄█▄▖▗▄▄▞▘  █  ▐▙▄▄▖▐▌  ▐▌▐▙▄▄▖▐▌ ▐▌▗▄▄▞▘

   Loop through each event list and executre callback

 */

bool html_event_listener_mouse_move(int eventType,
                                    const EmscriptenMouseEvent *mouseEvent,
                                    void *userData) {

  for (size_t i = 0; i < g_html_event.mouse_move.length; i++) {

    HTMLEventMouse *event = &g_html_event.mouse_move.entries[i];
    em_mouse_callback_func callback = event->callback;
    void *data = event->data;
    // pass down the parent arguments, exepct the userData get replaced by
    // callback data
    callback(eventType, mouseEvent, data);
  }

  return EM_FALSE;
}

bool html_event_listener_mouse_down(int eventType,
                                    const EmscriptenMouseEvent *mouseEvent,
                                    void *userData) {

  for (size_t i = 0; i < g_html_event.mouse_down.length; i++) {

    HTMLEventMouse *event = &g_html_event.mouse_down.entries[i];
    em_mouse_callback_func callback = event->callback;
    void *data = event->data;
    // pass down the parent arguments, exepct the userData get replaced by
    // callback data
    callback(eventType, mouseEvent, data);
  }

  return EM_TRUE;
}

bool html_event_listener_wheel(int eventType,
                               const EmscriptenWheelEvent *wheelEvent,
                               void *userData) {

  for (size_t i = 0; i < g_html_event.wheel.length; i++) {

    HTMLEventWheel *event = &g_html_event.wheel.entries[i];
    em_wheel_callback_func callback = event->callback;
    void *data = event->data;
    // pass down the parent arguments, exepct the userData get replaced by
    // callback data
    callback(eventType, wheelEvent, data);
  }

  return EM_TRUE;
}

bool html_event_listener_key_up(int eventType,
                                const EmscriptenKeyboardEvent *keyboardEvent,
                                void *userData) {

  for (size_t i = 0; i < g_html_event.wheel.length; i++) {

    HTMLEventKey *event = &g_html_event.key_up.entries[i];
    em_key_callback_func callback = event->callback;
    void *data = event->data;
    // pass down the parent arguments, exepct the userData get replaced by
    // callback data
    callback(eventType, keyboardEvent, data);
  }

  return EM_FALSE;
}

bool html_event_listener_key_down(int eventType,
                                  const EmscriptenKeyboardEvent *keyboardEvent,
                                  void *userData) {

  for (size_t i = 0; i < g_html_event.wheel.length; i++) {

    HTMLEventKey *event = &g_html_event.key_down.entries[i];
    em_key_callback_func callback = event->callback;
    void *data = event->data;
    // pass down the parent arguments, exepct the userData get replaced by
    // callback data
    callback(eventType, keyboardEvent, data);
  }

  return EM_FALSE;
}

