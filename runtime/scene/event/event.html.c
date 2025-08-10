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

  // traverse the active pipelines from the render mode and
  SceneRendererDrawMode draw_mode = scene_renderer_draw_mode(&scene->renderer);
  RenderPassLayout *layout = &scene->renderer.draw.layouts[draw_mode];

  // traverse render pass configs of the layout
  for (size_t i = 0; i < layout->length; i++) {
    RenderPassDrawList *draw_list = &layout->entries[i];
    for (size_t j = 0; j < draw_list->length; j++) {
      RenderPassDrawLayout *mesh_lists = &draw_list->entries[j];
      // update bind views of each meshes in each rende pass pipelines
      for (size_t k = 0; k < mesh_lists->meshes->length; k++) {

	// target mesh and shader
        Mesh *mesh = mesh_lists->meshes->entries[k];
        Shader *shader = mesh_lists->shader_callback(mesh);

	// get shader relative view index
        bind_group_index group_index = shader->pipeline->bindings.mvp.group;
        bind_index view_index = shader->pipeline->bindings.mvp.view;

	// generate new camera
        CameraUniform* cam = camera_uniform(scene->active_camera);

        shader_update_uniform(shader, group_index, view_index, (void *)&cam);
      }
    }
  }
}
