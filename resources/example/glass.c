#include "glass.h"

#include <cglm/types.h>
#include <stddef.h>

#include "backend/context.h"
#include "backend/renderer/reflection/draw.h"
#include "backend/renderer/render_pass/core.h"
#include "backend/resource_manager.h"
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
#include "runtime/shader/core.h"
#include "runtime/shader/update.h"
#include "runtime/systems/scene_system.h"
#include "runtime/texture/core.h"

void example_glass_probe_grid(Scene *scene, Renderer *renderer, bool debug) {

  SceneEditorMeshList *grid_probe =
      scene_system_add_probe_reflection_grid(scene, renderer,
                                             &(ProbeReflectionGridDescriptor){
                                                 .count = {3, 3, 3},
                                                 .scale = {30.0f, 30.0f, 30.0f},
                                             },
                                             NULL);

  Mesh *mesh = rem_new_mesh();

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

  scene_system_add_mesh(scene, renderer, mesh, NULL, SceneAddFlag_None);

  // TODO: Put this in the scene build for automation ?

  // link glass settings
  shader_update_uniform_data(mesh_shader(mesh, MeshShader_Texture), 1, 0,
                             &(GlassUniform){
                                 .color = {1.0f, 1.0f, 1.0f, 1.0f},
                                 .frost_scale = 80.0f,
                                 .frost_strength = 10.0f,
                                 .roughness = 0.145f,
                             },
                             ShaderUpdateFlag_None);

  // link probe lists (position, radius)
  Shader *shader = mesh_shader(mesh, MeshShader_Texture);
  shader_update_uniform_buffer(shader, shader->pipeline->bindings.probe->group,
                               shader->pipeline->bindings.probe->list,
                               ubo_buffer_handle(scene->ubo, UBOType_ProbeList),
                               0, ShaderUpdateFlag_ReleasePrevious);

  // link probe color texture
  shader_update_texture_view(
      mesh_shader(mesh, MeshShader_Texture),
      shader->pipeline->bindings.probe->group,
      shader->pipeline->bindings.probe->reflection_grid_texture,
      scene->probes.reflection_probe.pass.color.attachment.view,
      TEXTURE_FORMAT_OFFSCREEN, ShaderUpdateFlag_ReleasePrevious);

  // DELETEME (linked in scene add directly ??)
  shader_update_texture_view(
      mesh_shader(mesh, MeshShader_Texture),
      shader->pipeline->bindings.probe->group,
      shader->pipeline->bindings.probe->skybox_texture,
      scene_environment_skybox(&scene->environment)->view,
      TEXTURE_FORMAT_OFFSCREEN, ShaderUpdateFlag_ReleasePrevious);

  ProbeReflectionListDebug debug_options = {
      .scene_debug = &scene->debug,
      .max_views = 16,
  };

  // DELETEME ?
  probe_reflection_grid_list_draw(renderer,
                                  (void *)&scene->probes.reflection_probe);
}

void example_glass_probe_plane(Scene *scene, Renderer *renderer, bool debug) {

  const float scale = 20.0f;

  ProbeReflectionPlane *plane;
  SceneEditorMeshList *plane_probe = scene_system_add_probe_reflection_plane(
      scene, renderer,
      &(ProbeReflectionPlaneDescriptor){
          .far = 100.0f,
          .near = 0.1f,
          .normal = {0.0f, 1.0f, 0.0f},
          .scale = {scale + 5.0f, scale + 5.0f, scale + 5.0f},
          .distance = 3.0f,
          .camera = scene->active_camera,
      },
      &plane);

  Mesh *mesh = rem_new_mesh();
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

  scene_system_add_mesh(scene, renderer, mesh, NULL, SceneAddFlag_None);

  // link glass settings
  shader_update_uniform_data(mesh_shader(mesh, MeshShader_Texture), 1, 0,
                             &(GlassUniform){
                                 .color = {1.0f, 1.0f, 1.0f, 1.0f},
                                 .frost_scale = 20.0f,
                                 .frost_strength = 2.0f,
                                 .roughness = 0.145f,
                             },
                             ShaderUpdateFlag_None);
  // link UBO
  Shader *shader = mesh_shader(mesh, MeshShader_Texture);
  shader_update_uniform_buffer(shader, shader->pipeline->bindings.probe->group,
                               shader->pipeline->bindings.probe->list,
                               ubo_buffer_handle(scene->ubo, UBOType_ProbeList),
                               0, ShaderUpdateFlag_ReleasePrevious);

  mesh_shader_texture_bind_probe(mesh, plane, scene->ubo);
}
