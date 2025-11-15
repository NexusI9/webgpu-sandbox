#include "visibility_system.h"
#include "backend/renderer/batch.h"
#include "backend/renderer/core.h"
#include "runtime/engine/core.h"
#include "runtime/gizmo/core.h"
#include "runtime/systems/scene_system.h"

void visibility_system_show_mesh(Scene *scene, Renderer *renderer, Mesh *mesh) {

  renderer_show_mesh(renderer, RendererDrawMode_All, RendererLayer_All, mesh);

  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}

void visibility_system_hide_mesh(Scene *scene, Renderer *renderer, Mesh *mesh) {

  renderer_hide_mesh(renderer, RendererDrawMode_All, RendererLayer_All, mesh);

  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}

void visibility_system_show_mesh_ref_list(Scene *scene, Renderer *renderer,
                                          MeshRefList *list) {

  renderer_show_mesh_ref_list(renderer, RendererDrawMode_All, RendererLayer_All,
                              list);

  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}

void visibility_system_hide_mesh_ref_list(Scene *scene, Renderer *renderer,
                                          MeshRefList *list) {

  renderer_hide_mesh_ref_list(renderer, RendererDrawMode_All, RendererLayer_All,
                              list);

  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}

void visibility_system_toggle_mesh(Scene *scene, Renderer *renderer,
                                   Mesh *mesh) {

  renderer_visibility_toggle_mesh(renderer, RendererDrawMode_All,
                                  RendererLayer_All, mesh);

  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}
