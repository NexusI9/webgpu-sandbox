#include "event.html.h"
#include "emscripten/em_types.h"

static void scene_event_html_update_meshes(void *);

bool scene_event_html_mouse(int eventType, const EmscriptenMouseEvent *event,
                            void *data) {
  scene_event_html_update_meshes(data);
  return EM_FALSE;
}

bool scene_event_html_wheel(int eventType, const EmscriptenWheelEvent *event,
                            void *data) {
  scene_event_html_update_meshes(data);
  return EM_FALSE;
}

bool scene_event_html_key(int eventType, const EmscriptenKeyboardEvent *event,
                          void *data) {
  scene_event_html_update_meshes(data);
  return EM_FALSE;
}

/**
   Mousemove/wheel and key down implies that the camera moved, as a result we
   need to update scene meshes uniforms with the new camera data.
 */
void scene_event_html(Scene *scene) {

  // mouse event
  html_event_add_mouse_move(&(HTMLEventMouse){
      .callback = scene_event_html_mouse,
      .data = scene,
      .size = 0,
      .owner = scene->id,
  });

  // wheel events
  html_event_add_wheel(&(HTMLEventWheel){
      .callback = scene_event_html_wheel,
      .data = scene,
      .size = 0,
      .owner = scene->id,
  });

  // key events
  html_event_add_key_down(&(HTMLEventKey){
      .callback = scene_event_html_key,
      .data = scene,
      .size = 0,
      .owner = scene->id,
  });
}

void scene_event_html_update_meshes(void *data) {
  Scene *scene = (Scene *)data;

  // update mesh uniforms from active shader (Texture | Fixed)
  for (size_t i = 0; i < scene->meshes.length; i++)
    mesh_shader_active_update_views(&scene->meshes.entries[i],
                                    scene->active_camera, &scene->viewport);
}
