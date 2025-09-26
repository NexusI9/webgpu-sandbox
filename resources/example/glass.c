#include "glass.h"

#include <cglm/types.h>
#include <stddef.h>

#include "backend/context.h"
#include "backend/ssbo.h"
#include "backend/std_pipeline/core.h"
#include "backend/std_pipeline/render_shader/glass_probe_grid/glass_probe_grid.h"
#include "backend/ubo.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/mesh/shader/texture.h"
#include "runtime/mesh/transform.h"
#include "runtime/pipeline/render.h"
#include "runtime/primitive/core.h"
#include "runtime/primitive/icosphere.h"
#include "runtime/primitive/plane.h"
#include "runtime/probe/reflection/core.h"
#include "runtime/probe/reflection/grid.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"
#include "runtime/scene/environment/core.h"
#include "runtime/scene/renderer/render_pass/core.h"
#include "runtime/shader/core.h"
#include "runtime/shader/update.h"
#include "runtime/texture/core.h"

void example_glass_probe_grid(Scene *scene, bool debug) {

  SceneEditorObject *grid_probe =
      scene_add_probe_reflection_grid(scene,
                                      &(ProbeReflectionGridDescriptor){
                                          .count = {3, 3, 3},
                                          .scale = {30.0f, 30.0f, 30.0f},
                                      },
                                      NULL);

  Mesh *mesh = scene_new_mesh(scene);

  Primitive prim = primitive_icosphere();

  mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = &prim,
                                  .name = "Glass Probe Sphere",
                              });

  mesh_shader_create(mesh, &(ShaderCreateDescriptor){
                               .pipeline = std_render_pipeline(
                                   RenderPipelineType_GlassProbeGrid),
                               .name = "Glass Probe Sphere",
                           });

  mesh_set_position(mesh, (vec3){0.0f, 6.0f, 0.0f});
  mesh_set_rotation(mesh, (vec3){180.0f, 0.0f, 0.0f});
  mesh_set_scale(mesh, (vec3){3.0f, 3.0f, 3.0f});

  scene_add_mesh(scene, mesh, NULL, SceneAddFlag_None);

  // TODO: Put this in the scene build for automation ?

  // link glass settings
  shader_update_uniform_data(mesh_shader(mesh, MeshShader_Texture), 1, 0,
                             &(GlassUniform){
                                 .color = {1.0f, 1.0f, 1.0f, 1.0f},
                                 .frost_scale = 80.0f,
                                 .frost_strength = 10.0f,
                                 .roughness = 0.145f,
                             });

  // link probe lists (position, radius)
  shader_update_uniform_buffer(
      mesh_shader(mesh, MeshShader_Texture), 1, 1,
      ssbo_buffer_handle(&scene->renderer.ssbo, SSBOType_ProbeGridReflection),
      0, ShaderBufferLifetime_Release);

  // link UBO
  shader_update_uniform_buffer(mesh_shader(mesh, MeshShader_Texture), 1, 2,
                               ubo_buffer_handle(&scene->renderer.ubo), 0,
                               ShaderBufferLifetime_Release);

  // link probe color texture
  shader_update_texture_view(
      mesh_shader(mesh, MeshShader_Texture), 1, 3,
      scene->probes_reflection.pass.color.attachment.view,
      TEXTURE_FORMAT_OFFSCREEN);

  shader_update_texture_view(
      mesh_shader(mesh, MeshShader_Texture), 1, 5,
      scene_environment_skybox(&scene->environment)->view,
      TEXTURE_FORMAT_OFFSCREEN);

  ProbeReflectionListDebug debug_options = {
      .scene_debug = &scene->debug,
      .max_views = 16,
  };

  probe_reflection_grid_list_draw(&scene->probes_reflection,
                                  debug ? &debug_options : NULL);
}

void example_glass_probe_plane(Scene *scene, bool debug) {

  const float scale = 20.0f;

  ProbeReflectionPlane *plane;
  SceneEditorObject *plane_probe = scene_add_probe_reflection_plane(
      scene,
      &(ProbeReflectionPlaneDescriptor){
          .far = 100.0f,
          .near = 0.1f,
          .normal = {0.0f, 1.0f, 0.0f},
          .scale = {scale + 5.0f, scale + 5.0f, scale + 5.0f},
          .distance = 3.0f,
          .camera = scene->active_camera,
      },
      &plane);

  Mesh *mesh = scene_new_mesh(scene);
  Primitive prim = primitive_plane();

  mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = &prim,
                                  .name = "Glass Probe Plane",
                              });

  mesh_shader_create(mesh, &(ShaderCreateDescriptor){
                               .pipeline = std_render_pipeline(
                                   RenderPipelineType_GlassProbePlane),
                               .name = "Glass Probe Plane",
                           });

  mesh_set_position(mesh, (vec3){0.0f, 0.2f, 0.0f});
  mesh_set_rotation(mesh, (vec3){180.0f, 0.0f, 0.0f});
  mesh_set_scale(mesh, (vec3){scale, scale, scale});

  scene_add_mesh(scene, mesh, NULL, SceneAddFlag_None);

  // link glass settings
  shader_update_uniform_data(mesh_shader(mesh, MeshShader_Texture), 1, 0,
                             &(GlassUniform){
                                 .color = {1.0f, 1.0f, 1.0f, 1.0f},
                                 .frost_scale = 20.0f,
                                 .frost_strength = 2.0f,
                                 .roughness = 0.145f,
                             });
  // link UBO
  shader_update_uniform_buffer(mesh_shader(mesh, MeshShader_Texture), 1, 2,
                               ubo_buffer_handle(&scene->renderer.ubo), 0,
                               ShaderBufferLifetime_Release);

  mesh_shader_texture_bind_probe(mesh, plane, &scene->renderer.ssbo);
}
