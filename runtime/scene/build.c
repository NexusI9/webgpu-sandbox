#include "build.h"
#include "../material/material.h"
#include "../utils/system.h"
#include <stdint.h>

static void scene_build_mesh_list(Scene *, mesh_get_shader_callback,
                                  MeshRefList *);

static void scene_build_mesh_create_dynamic_shader(
    Scene *, mesh_create_dynamic_shader_callback, MeshRefList *);

static void scene_build_mesh_create_topology(Scene *,
                                             mesh_topology_create_callback,
                                             mesh_get_topology_callback,
                                             MeshRefList *);

static void scene_build_mesh_bind_views(Scene *, material_bind_views_callback,
                                        uint8_t, MeshRefList *);

/**
   Build meshes Texture shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_texture(Scene *scene) {
  VERBOSE_PRINT("======= BUILD TEXTURE SCENE ======\n");

  MeshRefList *unlit = &scene->layer.unlit;

  // process unlit
  scene_build_mesh_bind_views(scene, material_texture_bind_views,
                              SHADER_TEXTURE_BINDGROUP_VIEWS, unlit);

  MeshRefList *lit = &scene->layer.lit;

  // create wireframe topology
  scene_build_mesh_create_topology(scene, mesh_topology_wireframe_create,
                                   mesh_topology_wireframe, lit);

  // bind views
  scene_build_mesh_bind_views(scene, material_texture_bind_views,
                              SHADER_TEXTURE_BINDGROUP_VIEWS, lit);

  // bind lights
  for (int i = 0; i < lit->length; i++) {
    material_texture_bind_lights(lit->entries[i], &scene->lights,
                                 SHADER_TEXTURE_BINDGROUP_LIGHTS);
  }

  // draw solid meshes first
  scene_build_mesh_list(scene, mesh_shader_texture, lit);

  // draw transparent meshes then
  scene_build_mesh_list(scene, mesh_shader_texture, unlit);

  VERBOSE_PRINT("=======       DONE       ======\n");
}

/**
   Build meshes Solid shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_solid(Scene *scene) {
  VERBOSE_PRINT("======= BUILD SOLID SCENE ======\n");

  // create meshes' solid shader

  // bind views

  // draw solid meshes first
  scene_build_mesh_list(scene, mesh_shader_solid, &scene->layer.lit);
  // draw transparent meshes then
  scene_build_mesh_list(scene, mesh_shader_solid, &scene->layer.unlit);

  VERBOSE_PRINT("=======       DONE       ======\n");
}

/**
   Build meshes Wireframe shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_wireframe(Scene *scene) {
  VERBOSE_PRINT("======= BUILD WIREFRAME SCENE ======\n");

  MeshRefList *lit = &scene->layer.lit;

  // create wireframe topology
  scene_build_mesh_create_topology(scene, mesh_topology_wireframe_create,
                                   mesh_topology_wireframe, lit);

  // create meshes' wireframe shader
  scene_build_mesh_create_dynamic_shader(scene, mesh_create_wireframe_shader,
                                         lit);

  // bind views
  scene_build_mesh_bind_views(scene, material_wireframe_bind_views,
                              SHADER_WIREFRAME_BINDGROUP_VIEWS, lit);

  // build "solid" meshes first
  scene_build_mesh_list(scene, mesh_shader_wireframe, &scene->layer.lit);

  MeshRefList *unlit = &scene->layer.unlit;

  // create wireframe topology
  scene_build_mesh_create_topology(scene, mesh_topology_wireframe_create,
                                   mesh_topology_wireframe, unlit);

  scene_build_mesh_create_dynamic_shader(scene, mesh_create_wireframe_shader,
                                         unlit);
  // bine views
  scene_build_mesh_bind_views(scene, material_wireframe_bind_views,
                              SHADER_WIREFRAME_BINDGROUP_VIEWS, unlit);

  // draw transparent meshes then
  scene_build_mesh_list(scene, mesh_shader_wireframe, unlit);

  VERBOSE_PRINT("=======       DONE       ======\n");
}

/**
   Build meshes Shadow shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_shadow(Scene *scene) {
  VERBOSE_PRINT("======= BUILD SHADOW SCENE ======\n");

  // (shadow is bind process already during the renderer shadow pass)
  // draw solid meshes first
  scene_build_mesh_list(scene, mesh_shader_shadow, &scene->layer.lit);
  // draw transparent meshes then
  scene_build_mesh_list(scene, mesh_shader_shadow, &scene->layer.unlit);

  VERBOSE_PRINT("=======       DONE       ======\n");
}

/**
   Build Fixed mesh layer.
   Fixed layer use the Override shader* as default shader.
 */
void scene_build_fixed(Scene *scene) {
  VERBOSE_PRINT("======= BUILD FIXED SCENE ======\n");

  MeshRefList *fixed = &scene->layer.fixed;

  // create wireframe topology
  // scene_build_mesh_create_topology(scene, mesh_topology_wireframe_create,
  //                                mesh_topology_wireframe, fixed);

  // bind views
  scene_build_mesh_bind_views(scene, material_override_bind_views,
                              SHADER_FIXED_BINDGROUP_VIEWS, fixed);

  // build fixed
  scene_build_mesh_list(scene, mesh_shader_override, &scene->layer.fixed);

  VERBOSE_PRINT("=======       DONE       ======\n");
}

void scene_build_mesh_list(Scene *scene, mesh_get_shader_callback target_shader,
                           MeshRefList *mesh_list) {
  for (int i = 0; i < mesh_list->length; i++) {
    Mesh *current_mesh = mesh_list->entries[i];
    mesh_build(current_mesh, target_shader(current_mesh));
    shader_module_release(target_shader(current_mesh));
  }
}

/**
   Create mesh's extra shader for the dynamic rendering (wireframe/ shadow)
   Dynamic shaders are only used in lit and unlit scene layers
 */
void scene_build_mesh_create_dynamic_shader(
    Scene *scene, mesh_create_dynamic_shader_callback target_shader_creator,
    MeshRefList *mesh_list) {
  for (int i = 0; i < mesh_list->length; i++) {
    Mesh *current_mesh = mesh_list->entries[i];
    target_shader_creator(current_mesh);
  }
}

/**
   Create mesh's extra topology (wireframe/bounding box...)
   All extra topology are derived from the mesh's base topology and manipulate
   it accordingly to create the new topology.
 */
void scene_build_mesh_create_topology(
    Scene *scene, mesh_topology_create_callback topo_creator_callback,
    mesh_get_topology_callback topo_getter_callback, MeshRefList *mesh_list) {
  for (int i = 0; i < mesh_list->length; i++) {
    Mesh *mesh = mesh_list->entries[i];
    MeshTopology src_topo = mesh_topology_base_vertex(&mesh->topology.base);
    MeshTopology dest_topo = topo_getter_callback(mesh);
    
    topo_creator_callback(&src_topo, &dest_topo, mesh->device, mesh->queue);
  }
}

void scene_build_mesh_bind_views(Scene *scene,
                                 material_bind_views_callback bind_callback,
                                 uint8_t group_id, MeshRefList *mesh_list) {

  for (int i = 0; i < mesh_list->length; i++) {
    Mesh *mesh = mesh_list->entries[i];
    bind_callback(mesh, scene->active_camera, &scene->viewport, group_id);
  }
}
