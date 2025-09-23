#include "build.h"

#include <stddef.h>

#include "core.h"
#include "backend/ssbo.h"
#include "backend/ubo.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/mesh/shader/texture.h"
#include "runtime/mesh/topology/base.h"
#include "runtime/mesh/topology/boundbox.h"
#include "runtime/mesh/topology/core.h"
#include "runtime/mesh/topology/wireframe.h"
#include "runtime/pipeline/render.h"
#include "renderer/core.h"
#include "renderer/render_pass/core.h"
#include "runtime/mesh/core.h"
#include "backend/logger.h"

typedef void (*scene_builder_callback)(Scene *, Mesh *, const RenderPipeline *);

// pipeline builders
static inline void scene_build_mesh_texture(Scene *, Mesh *,
                                            const ScenePipeline);

static inline void scene_build_mesh_solid(Scene *, Mesh *, const ScenePipeline);

static inline void scene_build_mesh_wireframe(Scene *, Mesh *,
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
void scene_build_mesh(Scene *scene, Mesh *mesh, const ScenePipeline pipeline) {
  
  SSBOManager *ssbo = &scene->renderer.ssbo;
  UBOManager *ubo = &scene->renderer.ubo;

  const SceneRendererDrawMode draw_mode = scene->renderer.draw.mode;

  // Fixed rendering (NOT part of shader/topology creation automation, meaning
  // it's the developer responsibility to create the relative topology and
  // shaders.)
  if (pipeline >= ScenePipeline_Fixed_Background) {
    scene_build_mesh_fixed(scene, mesh, pipeline);
  } else {

    {
      // EDITORONLY
      // Build Boundbox & Wireframe by default for selection (maybe temporary
      // cause we may switch to an outline based visual for the selection)
      scene_build_mesh_boundbox(scene, mesh, pipeline);
    }

    // Dynamic rendering
    switch (draw_mode) {

    case SceneRendererDrawMode_Boundbox:
      // scene_build_mesh_boundbox(scene, mesh, pipeline);
      break;

    case SceneRendererDrawMode_Solid:
      scene_build_mesh_solid(scene, mesh, pipeline);
      break;

    case SceneRendererDrawMode_Wireframe:
      scene_build_mesh_wireframe(scene, mesh, pipeline);
      break;

    case SceneRendererDrawMode_Texture:
      scene_build_mesh_texture(scene, mesh, pipeline);
      break;
    }
  }
}

/**
   Build a mesh reference list. Useful for gizmos or any ref list in which
   objects fit in the same pipeline.
 */
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

  SSBOManager *ssbo = &scene->renderer.ssbo;
  UBOManager *ubo = &scene->renderer.ubo;

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                       &mesh->topology.boundbox);

  // bind views
  mesh_shader_build_mvp(mesh, MeshShader_Texture, ssbo);
  mesh_shader_build_mvp(mesh, MeshShader_Reflection, ssbo);

  if (pipeline &
      (ScenePipeline_Dynamic_Unlit | ScenePipeline_Dynamic_LitShadow |
       ScenePipeline_Dynamic_Lit)) {

    mesh_shader_texture_update_environment(mesh, scene->environment.skybox.view,
                                           ssbo);

    mesh_shader_texture_update_probes(
        mesh, scene->planes_reflection.pass.color.attachment.view,
        scene->probes_reflection.pass.color.attachment.view, ssbo);
  }
  if (pipeline &
      (ScenePipeline_Dynamic_LitShadow | ScenePipeline_Dynamic_Lit)) {

    mesh_shader_texture_update_lights(mesh, MeshShader_Texture, ubo, ssbo);
    mesh_shader_texture_update_lights(mesh, MeshShader_Reflection, ubo, ssbo);
  }
  if (pipeline == ScenePipeline_Dynamic_LitShadow) {
    mesh_shader_texture_bind_shadow_maps(
        mesh, scene->lights.point.shadow.pass.depth.attachment.view,
        scene->lights.spot.shadow.pass.depth.attachment.view);

    // create mesh shadow shader
    mesh_shader_create_shadow(mesh);
    mesh_shader_build_mp(mesh, MeshShader_Shadow, ssbo,
                         SSBOType_ViewProjection);
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
  mesh_shader_create_solid(mesh);

  // bind views
  mesh_shader_build_mvp(mesh, MeshShader_Solid, &scene->renderer.ssbo);
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
  mesh_topology_wireframe_create(&src_topo, dest_topo, mesh->device,
                                 mesh->queue);

  // create meshes' wireframe shader
  if (mesh_shader_create_wireframe(mesh) == MeshStatus_Success)
    mesh_shader_build_mvp(mesh, MeshShader_Wireframe, &scene->renderer.ssbo);
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
  mesh_topology_boundbox_create(&mesh->topology.base, mesh->model, dest_topo,
                                mesh->device, mesh->queue);

  // create meshes' wireframe shader
  mesh_shader_create_wireframe(mesh);

  // bind views
  mesh_shader_build_mvp(mesh, MeshShader_Wireframe, &scene->renderer.ssbo);
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

  // bind views
  mesh_shader_build_mvp(mesh, MeshShader_Fixed, &scene->renderer.ssbo);
}
