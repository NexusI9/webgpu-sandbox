#include "event.html.h"

#include <emscripten/html5.h>
#include <stddef.h>

#include "backend/ubo.h"
#include "emscripten/em_types.h"
#include "runtime/camera/core.h"
#include "runtime/camera/mode.h"
#include "runtime/html_event/add.h"
#include "runtime/html_event/core.h"
#include "runtime/input/core.h"
#include "runtime/probe/reflection/core.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/scene/core.h"

static void scene_event_html_update_meshes(Scene *);

static inline void scene_event_html_commons(Scene *);

bool scene_event_html_mouse(int eventType, const EmscriptenMouseEvent *event,
                            void *data) {

  if (g_input.locked & InputLockState_Mouse)
    return EM_FALSE;

  scene_event_html_commons((Scene *)data);
  return EM_FALSE;
}

bool scene_event_html_wheel(int eventType, const EmscriptenWheelEvent *event,
                            void *data) {

  if (g_input.locked & InputLockState_Mouse)
    return EM_FALSE;

  scene_event_html_commons((Scene *)data);
  return EM_FALSE;
}

bool scene_event_html_key(int eventType, const EmscriptenKeyboardEvent *event,
                          void *data) {

  if (g_input.locked & InputLockState_Mouse)
    return EM_FALSE;

  scene_event_html_commons((Scene *)data);
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

/**
   Functions called during all events (key/wheel/mouse)
 */
void scene_event_html_commons(Scene *scene) {

  scene_event_html_update_meshes(scene);

  // update camera controls
  Camera *camera = scene->active_camera;
  camera_mode_controller[camera->mode](camera);
  ubo_update_queue_insert(scene->ubo, UBOType_Camera,
                          camera->ubo_slot.id);

  // update planar reflections probes views
  for (size_t i = 0; i < scene->probes.reflection_plane.length; i++) {
    ProbeReflectionPlane *probe = &scene->probes.reflection_plane.entries[i];

    // update CPU side
    probe_reflection_plane_update_camera(probe);
    probe_reflection_plane_update_uniform(probe);

    // add to GPU update Queue
    ubo_update_queue_insert(scene->ubo, UBOType_Camera,
                            probe->ubo_camera.id);
  }

  // add to GPU update Queue
  ubo_update_queue_insert(scene->ubo, UBOType_ProbeList,
                          scene->probes.ubo_slot.id);
}

void scene_event_html_update_meshes(Scene *data) {
  Scene *scene = (Scene *)data;
  /*
    DELETEME ??
  // traverse the active pipelines from the render mode and
  RendererDrawMode draw_mode = renderer_draw_mode(&scene->renderer);
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

        shader_update_uniform_data(shader, group_index, view_index, (void
  *)&cam);
      }
    }
  }
  */
}
