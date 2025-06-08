#include "build.h"
#include "../material/material.h"
#include "../utils/system.h"

static void scene_build_mesh_list(Scene *, mesh_get_shader_callback,
                                  MeshRefList *);

static void scene_build_mesh_create_dynamic_shader(
    Scene *, mesh_create_dynamic_shader_callback, MeshRefList *);

/**
   Build meshes Texture shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_texture(Scene *scene) {
  VERBOSE_PRINT("======= BUILD TEXTURE SCENE ======\n");

  // bind unlit views
  for (int i = 0; i < scene->layer.unlit.length; i++)
    material_texture_bind_views(scene->layer.unlit.entries[i],
                                scene->active_camera, &scene->viewport,
                                SHADER_TEXTURE_BINDGROUP_VIEWS);

  // bind lit views & lights
  for (int i = 0; i < scene->layer.lit.length; i++) {
    // views
    material_texture_bind_views(scene->layer.lit.entries[i],
                                scene->active_camera, &scene->viewport,
                                SHADER_TEXTURE_BINDGROUP_VIEWS);
    // lights
    material_texture_bind_lights(scene->layer.lit.entries[i], &scene->lights,
                                 SHADER_TEXTURE_BINDGROUP_LIGHTS);
  }

  // draw solid meshes first
  scene_build_mesh_list(scene, mesh_shader_texture, &scene->layer.lit);

  // draw transparent meshes then
  scene_build_mesh_list(scene, mesh_shader_texture, &scene->layer.unlit);

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

  // create meshes' wireframe shader
  scene_build_mesh_create_dynamic_shader(scene, mesh_create_wireframe_shader,
                                         &scene->layer.lit);

  // bind views

  // unlit
  for (int i = 0; i < scene->layer.unlit.length; i++)
    material_wireframe_bind_views(scene->layer.unlit.entries[i],
                                  scene->active_camera, &scene->viewport,
                                  SHADER_WIREFRAME_BINDGROUP_VIEWS);

  // lit
  for (int i = 0; i < scene->layer.lit.length; i++)
    material_wireframe_bind_views(scene->layer.lit.entries[i],
                                  scene->active_camera, &scene->viewport,
                                  SHADER_WIREFRAME_BINDGROUP_VIEWS);

  // draw solid meshes first
  scene_build_mesh_list(scene, mesh_shader_wireframe, &scene->layer.lit);

  // draw transparent meshes then
  scene_build_mesh_create_dynamic_shader(scene, mesh_create_wireframe_shader,
                                         &scene->layer.unlit);
  scene_build_mesh_list(scene, mesh_shader_wireframe, &scene->layer.unlit);

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

  // bind views
  for (int i = 0; i < scene->layer.fixed.length; i++)
    material_texture_bind_views(scene->layer.fixed.entries[i],
                                scene->active_camera, &scene->viewport,
                                SHADER_FIXED_BINDGROUP_VIEWS);

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
