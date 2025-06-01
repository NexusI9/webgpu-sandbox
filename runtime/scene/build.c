#include "build.h"
#include "../utils/system.h"

static void scene_build_mesh_list(Scene *, mesh_get_shader_callback,
                                  MeshRefList *);

/**
   Build meshes Texture shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_texture(Scene *scene) {
  VERBOSE_PRINT("======= BUILD TEXTURE SCENE ======\n");
  // draw solid meshes first
  scene_build_mesh_list(scene, mesh_shader_texture, &scene->layer.lit);
  // draw transparent meshes then
  scene_build_mesh_list(scene, mesh_shader_texture, &scene->layer.unlit);
}

/**
   Build meshes Solid shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_solid(Scene *scene) {
  VERBOSE_PRINT("======= BUILD SOLID SCENE ======\n");
  // draw solid meshes first
  scene_build_mesh_list(scene, mesh_shader_solid, &scene->layer.lit);
  // draw transparent meshes then
  scene_build_mesh_list(scene, mesh_shader_solid, &scene->layer.unlit);
}

/**
   Build meshes Wireframe shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_wireframe(Scene *scene) {
  VERBOSE_PRINT("======= BUILD WIREFRAME SCENE ======\n");
  // draw solid meshes first
  scene_build_mesh_list(scene, mesh_shader_wireframe, &scene->layer.lit);
  // draw transparent meshes then
  scene_build_mesh_list(scene, mesh_shader_wireframe, &scene->layer.unlit);
}

/**
   Build meshes Shadow shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_shadow(Scene *scene) {
  VERBOSE_PRINT("======= BUILD SHADOW SCENE ======\n");
  // draw solid meshes first
  scene_build_mesh_list(scene, mesh_shader_shadow, &scene->layer.lit);
  // draw transparent meshes then
  scene_build_mesh_list(scene, mesh_shader_shadow, &scene->layer.unlit);
}

/**
   Build Fixed mesh layer.
   Fixed layer use the Texture shader as default shader (preventing creating an
   additional persisten unsued Fixed Shader in Mesh).
 */
void scene_build_fixed(Scene *scene) {
  VERBOSE_PRINT("======= BUILD FIXED SCENE ======\n");
  scene_build_mesh_list(scene, mesh_shader_texture, &scene->layer.fixed);
}

void scene_build_mesh_list(Scene *scene, mesh_get_shader_callback target_shader,
                           MeshRefList *mesh_list) {
  for (int i = 0; i < mesh_list->length; i++) {
    Mesh *current_mesh = mesh_list->entries[i];
    mesh_build(current_mesh, target_shader(current_mesh));
    shader_module_release(target_shader(current_mesh));
  }
}
