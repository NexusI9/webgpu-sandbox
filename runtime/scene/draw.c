#include "draw.h"

static void scene_draw_mesh_list(Scene *, mesh_get_vertex_callback,
                                 mesh_get_shader_callback,
                                 WGPURenderPassEncoder *, MeshRefList *);


void scene_draw_mesh_list(Scene *scene, mesh_get_vertex_callback target_vertex,
                          mesh_get_shader_callback target_shader,
                          WGPURenderPassEncoder *render_pass,
                          MeshRefList *mesh_list) {

  // loop through mesh list and draw meshes
  for (int i = 0; i < mesh_list->length; i++) {
    Mesh *current_mesh = mesh_list->entries[i];
    mesh_draw(target_vertex(current_mesh), target_shader(current_mesh),
              render_pass, scene->active_camera, &scene->viewport);
  }
}

// TODO: Simplify the overall scene draw/build process
void scene_draw_texture(Scene *scene, WGPURenderPassEncoder *render_pass) {

  // update camera
  camera_draw(scene->active_camera);

  // draw solid meshes first
  scene_draw_mesh_list(scene, mesh_topology_base, mesh_shader_texture,
                       render_pass, &scene->layer.lit);
  // draw transparent meshes then
  scene_draw_mesh_list(scene, mesh_topology_base, mesh_shader_texture,
                       render_pass, &scene->layer.unlit);
}

void scene_draw_fixed(Scene *scene, WGPURenderPassEncoder *render_pass) {

  // update camera
  camera_draw(scene->active_camera);

  // draw fixed mesh
  scene_draw_mesh_list(scene, mesh_topology_base, mesh_shader_texture,
                       render_pass, &scene->layer.fixed);
}

void scene_draw_shadow(Scene *scene, WGPURenderPassEncoder *render_pass) {
  // onlid draw solid/lit meshes
  scene_draw_mesh_list(scene, mesh_topology_base, mesh_shader_shadow, render_pass,
                       &scene->layer.lit);
}

void scene_draw_wireframe(Scene *scene, WGPURenderPassEncoder *render_pass) {

  // update camera
  camera_draw(scene->active_camera);

  // draw solid meshes first
  scene_draw_mesh_list(scene, mesh_topology_wireframe, mesh_shader_wireframe,
                       render_pass, &scene->layer.lit);
  // draw solid meshes then
  scene_draw_mesh_list(scene, mesh_topology_wireframe, mesh_shader_wireframe,
                       render_pass, &scene->layer.unlit);
}

void scene_draw_solid(Scene *scene, WGPURenderPassEncoder *render_pass) {

  // update camera
  camera_draw(scene->active_camera);

  // draw solid meshes first
  scene_draw_mesh_list(scene, mesh_topology_base, mesh_shader_solid, render_pass,
                       &scene->layer.lit);
  // draw solid meshes then
  scene_draw_mesh_list(scene, mesh_topology_base, mesh_shader_solid, render_pass,
                       &scene->layer.unlit);
}

