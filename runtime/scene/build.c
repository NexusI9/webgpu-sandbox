#include "build.h"

#include <stddef.h>

#include "backend/logger.h"
#include "backend/ubo.h"
#include "core.h"
#include "renderer/core.h"
#include "renderer/render_pass/core.h"
#include "runtime/geometry/line/core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/mesh/shader/texture.h"
#include "runtime/mesh/topology/base.h"
#include "runtime/mesh/topology/boundbox.h"
#include "runtime/mesh/topology/core.h"
#include "runtime/mesh/topology/wireframe.h"
#include "runtime/pipeline/render.h"
#include "utils/math.h"

typedef void (*scene_builder_callback)(Scene *, Mesh *, const RenderPipeline *);

// pipeline builders
static inline void scene_build_mesh_texture(Scene *, Mesh *,
                                            const ScenePipeline);

static inline void scene_build_mesh_solid(Scene *, Mesh *, const ScenePipeline);

static inline void scene_build_mesh_wireframe(Scene *, Mesh *,
                                              const ScenePipeline);

static inline void scene_build_mesh_outline(Scene *, Mesh *,
                                            const ScenePipeline);

static inline void scene_build_mesh_fixed(Scene *, Mesh *, const ScenePipeline);

static inline void scene_build_mesh_boundbox(Scene *, Mesh *,
                                             const ScenePipeline);

/**
   ▗▄▄▄ ▗▄▄▄▖ ▗▄▄▖▗▄▄▖  ▗▄▖▗▄▄▄▖▗▄▄▖▗▖ ▗▖▗▄▄▄▖▗▄▄▖
   ▐▌  █  █  ▐▌   ▐▌ ▐▌▐▌ ▐▌ █ ▐▌   ▐▌ ▐▌▐▌   ▐▌ ▐▌
   ▐▌  █  █   ▝▀▚▖▐▛▀▘ ▐▛▀▜▌ █ ▐▌   ▐▛▀▜▌▐▛▀▀▘▐▛▀▚▖
   ▐▙▄▄▀▗▄█▄▖▗▄▄▞▘▐▌   ▐▌ ▐▌ █ ▝▚▄▄▖▐▌ ▐▌▐▙▄▄▖▐▌ ▐▌

   Build the mesh according to the current scene rendere mode and pipeline in
   which the mesh will be added to.
 */
SceneStatus scene_build_mesh(Scene *scene, Mesh *mesh,
                             const ScenePipeline pipeline) {

  UBOManager *ubo = &scene->renderer.ubo;

  if (pipeline >= ScenePipeline_Fixed_Background) {
    /*
      === Fixed rendering ===

     (NOT part of shader/topology creation automation, meaning
     it's the developer responsibility to create the relative topology and
     shaders.)
    */

    scene_build_mesh_fixed(scene, mesh, pipeline);

  } else {

    {
      // EDITORONLY
      scene_build_mesh_outline(scene, mesh, pipeline);
    }

    {
      scene_build_mesh_boundbox(scene, mesh, pipeline);
      scene_build_mesh_solid(scene, mesh, pipeline);
      scene_build_mesh_wireframe(scene, mesh, pipeline);
      scene_build_mesh_texture(scene, mesh, pipeline);
    }
  }

  return SceneStatus_Success;
}

void scene_build_mesh_ref_list(Scene *scene, MeshRefList *list,
                               const ScenePipeline pipeline) {
  for (size_t i = 0; i < list->length; i++)
    scene_build_mesh(scene, list->entries[i], pipeline);
}

/**
  ▗▄▄▖ ▗▖ ▗▖▗▄▄▄▖▗▖   ▗▄▄▄ ▗▄▄▄▖▗▄▄▖  ▗▄▄▖
  ▐▌ ▐▌▐▌ ▐▌  █  ▐▌   ▐▌  █▐▌   ▐▌ ▐▌▐▌
  ▐▛▀▚▖▐▌ ▐▌  █  ▐▌   ▐▌  █▐▛▀▀▘▐▛▀▚▖ ▝▀▚▖
  ▐▙▄▞▘▝▚▄▞▘▗▄█▄▖▐▙▄▄▖▐▙▄▄▀▐▙▄▄▖▐▌ ▐▌▗▄▄▞▘

   Build meshes Texture shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_mesh_texture(Scene *scene, Mesh *mesh,
                              const ScenePipeline pipeline) {

#ifdef VERBOSE_BUILDING_PHASE
  logger_add(LoggerFlag_MeshBuild, "Texture %s", mesh->name);
#endif

  UBOManager *ubo = &scene->renderer.ubo;

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                       &mesh->topology.boundbox);

  // bind views
  mesh_shader_build_mvp(mesh, MeshShader_Texture, ubo);
  mesh_shader_build_mvp(mesh, MeshShader_Reflection, ubo);

  if (pipeline &
      (ScenePipeline_Dynamic_Unlit | ScenePipeline_Dynamic_LitAlpha |
       ScenePipeline_Dynamic_LitShadow | ScenePipeline_Dynamic_Lit)) {

    mesh_shader_texture_update_environment(
        mesh, scene->environment.skybox.view,
        (SceneEnvironmentUniform *)scene->environment.ubo_slot.uniform, ubo);

    mesh_shader_texture_update_probes(
        mesh, scene->probes.reflection_plane.pass.color.attachment.view,
        scene->probes.reflection_probe.pass.color.attachment.view, ubo);
  }

  if (pipeline & (ScenePipeline_Dynamic_LitShadow | ScenePipeline_Dynamic_Lit |
                  ScenePipeline_Dynamic_LitAlpha)) {

    mesh_shader_texture_update_lights(mesh, MeshShader_Texture, ubo);
    mesh_shader_texture_update_lights(mesh, MeshShader_Reflection, ubo);
  }

  if (pipeline &
      (ScenePipeline_Dynamic_LitShadow | ScenePipeline_Dynamic_LitAlpha)) {

    mesh_shader_texture_update_shadow_maps(
        mesh, scene->lights.point.shadow.pass.depth.attachment.view,
        scene->lights.spot.shadow.pass.depth.attachment.view);

    // create mesh shadow shader
    mesh_shader_create_standard(mesh, MeshShader_Shadow);

    mesh_shader_build_mp(mesh, MeshShader_Shadow, ubo, UBOType_ViewProjection);
  }
}

/**
   Build meshes Solid shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_mesh_solid(Scene *scene, Mesh *mesh,
                            const ScenePipeline pipeline) {

#ifdef VERBOSE_BUILDING_PHASE
  logger_add(LoggerFlag_MeshBuild, "Solid %s", mesh->name);
#endif

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                       &mesh->topology.boundbox);

  // create meshes' solid shader
  mesh_shader_create_standard(mesh, MeshShader_Solid);

  // bind views
  mesh_shader_build_mvp(mesh, MeshShader_Solid, &scene->renderer.ubo);
}

/**
   Build meshes Solid shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_mesh_outline(Scene *scene, Mesh *mesh,
                              const ScenePipeline pipeline) {

#ifdef VERBOSE_BUILDING_PHASE
  logger_add(LoggerFlag_MeshBuild, "Outline %s", mesh->name);
#endif

  // create meshes' solid shader
  if (mesh_shader_create_standard(mesh, MeshShader_Outline) ==
      MeshStatus_Success)
    mesh_shader_build_mvp(mesh, MeshShader_Outline, &scene->renderer.ubo);

  if (mesh_shader_create_standard(mesh, MeshShader_Stencil) ==
      MeshStatus_Success)
    mesh_shader_build_mvp(mesh, MeshShader_Stencil, &scene->renderer.ubo);
}

/**
   Build meshes Wireframe shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_mesh_wireframe(Scene *scene, Mesh *mesh,
                                const ScenePipeline pipeline) {

#ifdef VERBOSE_BUILDING_PHASE
  logger_add(LoggerFlag_MeshBuild, "Wireframe %s", mesh->name);
#endif

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                       &mesh->topology.boundbox);

  // create wireframe topology
  MeshTopology src_topo = mesh_topology_base_vertex(&mesh->topology.base);
  MeshTopologyWireframe *dest_topo = &mesh->topology.wireframe;
  mesh_topology_wireframe_create(&src_topo, dest_topo);

  if (mesh_shader_create_standard(mesh, MeshShader_Wireframe) ==
      MeshStatus_Success) {

    float line_thickness = LINE_THICKNESS_BASE;
    shader_update_uniform_data(mesh_shader(mesh, MeshShader_Wireframe), 1, 1,
                               (void *)&line_thickness, ShaderUpdateFlag_None);

    shader_update_uniform_data(mesh_shader(mesh, MeshShader_Wireframe), 1, 0,
                               &(color){0.0f, 0.0f, 0.0f, 1.0f},
                               ShaderUpdateFlag_None);

    mesh_shader_build_mvp(mesh, MeshShader_Wireframe, &scene->renderer.ubo);
  }
}

/**
   Build meshes Boundbox shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_mesh_boundbox(Scene *scene, Mesh *mesh,
                               const ScenePipeline pipeline) {

#ifdef VERBOSE_BUILDING_PHASE
  logger_add(LoggerFlag_MeshBuild, "Boundbox %s", mesh->name);
#endif

  // create full boundbox topology
  MeshTopologyBoundbox *dest_topo = &mesh->topology.boundbox;
  mesh_topology_boundbox_create(&mesh->topology.base, mesh->model, dest_topo);

  // create meshes' wireframe shader
  if (mesh_shader_create_standard(mesh, MeshShader_Wireframe) ==
      MeshStatus_Success) {

    float line_thickness = LINE_THICKNESS_BASE;
    shader_update_uniform_data(mesh_shader(mesh, MeshShader_Wireframe), 1, 1,
                               (void *)&line_thickness, ShaderUpdateFlag_None);

    // set wireframe random color
    shader_update_uniform_data(mesh_shader(mesh, MeshShader_Wireframe), 1, 0,
                               &(color){0.0f, 0.0f, 0.0f, 1.0f},
                               ShaderUpdateFlag_None);

    mesh_shader_build_mvp(mesh, MeshShader_Wireframe, &scene->renderer.ubo);
  }
}

/**
   Build Fixed mesh layer.
   Fixed layer use the Override shader* as default shader.
 */
void scene_build_mesh_fixed(Scene *scene, Mesh *mesh,
                            const ScenePipeline pipeline) {

#ifdef VERBOSE_BUILDING_PHASE
  logger_add(LoggerFlag_MeshBuild, "Fixed %s", mesh->name);
#endif

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                       &mesh->topology.boundbox);
  mesh_shader_build_mvp(mesh, MeshShader_Fixed, &scene->renderer.ubo);
}
