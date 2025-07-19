#include "draw.h"

static void scene_draw_mesh_list(Scene *, mesh_get_topology_callback,
                                 mesh_get_shader_callback,
                                 WGPURenderPassEncoder *, MeshRefList *);

void scene_draw_mesh_list(Scene *scene,
                          mesh_get_topology_callback target_topology,
                          mesh_get_shader_callback target_shader,
                          WGPURenderPassEncoder *render_pass,
                          MeshRefList *mesh_list) {

  // loop through mesh list and draw meshes
  for (int i = 0; i < mesh_list->length; i++) {
    Mesh *current_mesh = mesh_list->entries[i];
    mesh_draw(target_topology(current_mesh), target_shader(current_mesh),
              render_pass);
  }
}

// TODO: Simplify the overall scene draw/build process
void scene_draw_texture(Scene *scene, WGPURenderPassEncoder *render_pass) {

  // update camera
  camera_draw(scene->active_camera);

  // set pipelines order
  MeshRefList *pipelines[3] = {
      &scene->pipelines.background,
      &scene->pipelines.lit,
      &scene->pipelines.unlit,
  };

  for (size_t l = 0; l < 3; l++) {
    MeshRefList *pipeline = pipelines[l];
    scene_draw_mesh_list(scene, mesh_topology_base, mesh_shader_texture,
                         render_pass, pipeline);
  }

  // draw fixed mesh
  scene_draw_fixed(scene, render_pass);
}

void scene_draw_fixed(Scene *scene, WGPURenderPassEncoder *render_pass) {

  // update camera
  camera_draw(scene->active_camera);

  // draw fixed mesh
  scene_draw_mesh_list(scene, mesh_topology_override, mesh_shader_override,
                       render_pass, &scene->pipelines.fixed);
}

void scene_draw_shadow(Scene *scene, WGPURenderPassEncoder *render_pass) {
  // onlid draw solid/lit meshes
  scene_draw_mesh_list(scene, mesh_topology_base, mesh_shader_shadow,
                       render_pass, &scene->pipelines.lit);
}

void scene_draw_wireframe(Scene *scene, WGPURenderPassEncoder *render_pass) {

  // update camera
  camera_draw(scene->active_camera);

  // set pipelines order
  MeshRefList *pipelines[2] = {
      &scene->pipelines.lit,
      &scene->pipelines.unlit,
  };

  for (size_t l = 0; l < 2; l++) {
    MeshRefList *pipeline = pipelines[l];
    scene_draw_mesh_list(scene, mesh_topology_wireframe, mesh_shader_wireframe,
                         render_pass, pipeline);
  }

  // draw fixed mesh
  scene_draw_fixed(scene, render_pass);
}

void scene_draw_solid(Scene *scene, WGPURenderPassEncoder *render_pass) {

  // update camera
  camera_draw(scene->active_camera);

  // set pipelines order
  MeshRefList *pipelines[2] = {
      &scene->pipelines.lit,
      &scene->pipelines.unlit,
  };

  for (size_t l = 0; l < 2; l++) {
    MeshRefList *pipeline = pipelines[l];
    scene_draw_mesh_list(scene, mesh_topology_base, mesh_shader_solid,
                         render_pass, pipeline);
  }

  // draw fixed mesh
  scene_draw_fixed(scene, render_pass);
}

void scene_draw_boundbox(Scene *scene, WGPURenderPassEncoder *render_pass) {

  // update camera
  camera_draw(scene->active_camera);

  // set pipelines order
  MeshRefList *pipelines[2] = {
      &scene->pipelines.lit,
      &scene->pipelines.unlit,
  };

  for (size_t l = 0; l < 2; l++) {
    MeshRefList *pipeline = pipelines[l];
    scene_draw_mesh_list(scene, mesh_topology_boundbox, mesh_shader_wireframe,
                         render_pass, pipeline);
  }

  // draw fixed mesh
  scene_draw_fixed(scene, render_pass);
}

/**
   Draw the selection pipeline. Note that the selection pipeline use the
   boundbox build.

   Pipeline Selection  ==>  Build BoundBox  ==>  Draw Selection
 */
void scene_draw_selection(Scene *scene, WGPURenderPassEncoder *render_pass) {

  scene_draw_mesh_list(scene, mesh_topology_boundbox, mesh_shader_wireframe,
                       render_pass, &scene->pipelines.selection);
}
