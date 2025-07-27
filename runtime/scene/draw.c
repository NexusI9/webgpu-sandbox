#include "draw.h"
#include "../backend/renderer/renderer.h"
#include "../camera/camera.h"

/**
   Draw callback added to the Scene Renderer draw callbacks.
   Called before the scene renderer draw layouts.

   Basically udpate the camera matrix based on its mode and user input.
 */
void scene_camera_draw_callback(void *data) {

  Camera *cast_camera = (Camera *)data;

  // update camera
  camera_draw(cast_camera);
}

/**
   Draw the meshes of a given list along with a target topology and shader.
   The function will be call with attributes coming from the renderer
   draw_callbacks array.

   Using topology and shader callbacks allow greater flexibility when it comes
   to the different display modes.
 */
static int t = 0;
void scene_layout_draw_callback(void *data) {

  // cast data to renderer
  SceneRenderer *cast_renderer = (SceneRenderer *)data;

  // retrieve mode
  SceneRendererDrawMode mode = cast_renderer->draw.mode;
  RenderPassLayout *pass_layout = &cast_renderer->draw.layouts[mode];
  WGPURenderPassEncoder *render_pass = &cast_renderer->wgpu.render_pass;

  for (size_t i = 0; i < pass_layout->length; i++) {

    // retrieve each render pass entries (mesh/topo/shader)
    RenderPassDrawLayoutList *layout_list = &pass_layout->entries[i];

    // loop through mesh lists and draw meshes
    for (size_t j = 0; j < layout_list->length; j++) {

      // retrieve layout
      RenderPassDrawLayout *layout = &layout_list->entries[j];
      mesh_get_topology_callback target_topology = layout->topology_callback;
      mesh_get_shader_callback target_shader = layout->shader_callback;
      MeshRefList *meshes = layout->meshes;

      // draw mesh with layout callbacks
      for (size_t k = 0; k < meshes->length; k++) {
        Mesh *mesh = meshes->entries[k];
        mesh_draw(target_topology(mesh), target_shader(mesh), render_pass);
      }
    }
  }
}
