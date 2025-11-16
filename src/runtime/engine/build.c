#include "build.h"
#include "backend/renderer/batch.h"
#include "runtime/engine/core.h"
#include "runtime/geometry/line/core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/mesh/shader/texture.h"
#include "runtime/pipeline/render.h"

// pipeline builders
static inline void engine_build_mesh_texture(Scene *, Mesh *,
                                             const RendererBatchFlag);
static inline void engine_build_mesh_solid(Scene *, Mesh *);
static inline void engine_build_mesh_wireframe(Scene *, Mesh *);
static inline void engine_build_mesh_outline(Scene *, Mesh *);
static inline void engine_build_mesh_boundbox(Scene *, Mesh *);

/**
   ▗▄▄▄ ▗▄▄▄▖ ▗▄▄▖▗▄▄▖  ▗▄▖▗▄▄▄▖▗▄▄▖▗▖ ▗▖▗▄▄▄▖▗▄▄▖
   ▐▌  █  █  ▐▌   ▐▌ ▐▌▐▌ ▐▌ █ ▐▌   ▐▌ ▐▌▐▌   ▐▌ ▐▌
   ▐▌  █  █   ▝▀▚▖▐▛▀▘ ▐▛▀▜▌ █ ▐▌   ▐▛▀▜▌▐▛▀▀▘▐▛▀▚▖
   ▐▙▄▄▀▗▄█▄▖▗▄▄▞▘▐▌   ▐▌ ▐▌ █ ▝▚▄▄▖▐▌ ▐▌▐▙▄▄▖▐▌ ▐▌

   Build the mesh according to the current scene rendere mode and pipeline in
   which the mesh will be added to.
 */
EngineStatus engine_build_mesh(Engine *engine, Mesh *mesh,
                               const RendererBatchFlag flag) {

  Scene *scene = engine_get_active_scene(engine);
  Renderer *renderer = engine_get_renderer(engine);
  UBOManager *ubo = scene->ubo;

  // EDITORONLY
  engine_build_mesh_outline(scene, mesh);
  engine_build_mesh_texture(scene, mesh, flag);

  // only build those for dynamic meshes
  if ((flag & RendererBatchFlag_Fixed) == 0) {
    engine_build_mesh_boundbox(scene, mesh);
    engine_build_mesh_solid(scene, mesh);
    engine_build_mesh_wireframe(scene, mesh);
  }

  return EngineStatus_Success;
}

/**
  ▗▄▄▖ ▗▖ ▗▖▗▄▄▄▖▗▖   ▗▄▄▄ ▗▄▄▄▖▗▄▄▖  ▗▄▄▖
  ▐▌ ▐▌▐▌ ▐▌  █  ▐▌   ▐▌  █▐▌   ▐▌ ▐▌▐▌
  ▐▛▀▚▖▐▌ ▐▌  █  ▐▌   ▐▌  █▐▛▀▀▘▐▛▀▚▖ ▝▀▚▖
  ▐▙▄▞▘▝▚▄▞▘▗▄█▄▖▐▙▄▄▖▐▙▄▄▀▐▙▄▄▖▐▌ ▐▌▗▄▄▞▘

   Build meshes Texture shader in each scene list
   Establish pipeline from previously set bind groups
 */
void engine_build_mesh_texture(Scene *scene, Mesh *mesh,
                               const RendererBatchFlag flags) {

#ifdef VERBOSE_BUILDING_PHASE
  logger_add(LoggerFlag_MeshBuild, "Texture %s", mesh->name);
#endif

  UBOManager *ubo = scene->ubo;

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                       &mesh->topology.boundbox);

  // bind views
  mesh_shader_build_mvp(mesh, MeshShader_Texture, ubo);

  /*
  TODO: For now although we use a hash based pipeline routing it is still a
  pre-configured 1 to 1 configuration where 1 renderer pipeline = 1
  configuration. In the future make it more versatile by creating the
  configuration on the fly.

  In this case we need to retrieve the renderer batch configuration to get the
  pipeline configuration flags as to determine if we need to bind lights,
  shadow maps or reflections relative resources.
 */
  if (RendererBatchFlag_Reflection & flags) {

    mesh_shader_build_mvp(mesh, MeshShader_Reflection, ubo);

    mesh_shader_texture_update_environment(
        mesh, scene->environment.skybox.view,
        (SceneEnvironmentUniform *)scene->environment.ubo_slot.uniform, ubo);

    mesh_shader_texture_update_probes(
        mesh, scene->probes.reflection_plane.pass.color.attachment.view,
        scene->probes.reflection_probe.pass.color.attachment.view, ubo);
  }

  if (RendererBatchFlag_Lit & flags) {

    mesh_shader_texture_update_lights(mesh, MeshShader_Texture, ubo);
    mesh_shader_texture_update_lights(mesh, MeshShader_Reflection, ubo);
  }

  if (RendererBatchFlag_Shadow & flags) {

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
void engine_build_mesh_solid(Scene *scene, Mesh *mesh) {

#ifdef VERBOSE_BUILDING_PHASE
  logger_add(LoggerFlag_MeshBuild, "Solid %s", mesh->name);
#endif

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                       &mesh->topology.boundbox);

  // create meshes' solid shader
  mesh_shader_create_standard(mesh, MeshShader_Solid);

  // bind views
  mesh_shader_build_mvp(mesh, MeshShader_Solid, scene->ubo);
}

/**
   Build meshes Solid shader in each scene list
   Establish pipeline from previously set bind groups
 */
void engine_build_mesh_outline(Scene *scene, Mesh *mesh) {

#ifdef VERBOSE_BUILDING_PHASE
  logger_add(LoggerFlag_MeshBuild, "Outline %s", mesh->name);
#endif

  // create meshes' solid shader
  if (mesh_shader_create_standard(mesh, MeshShader_Outline) ==
      MeshStatus_Success)
    mesh_shader_build_mvp(mesh, MeshShader_Outline, scene->ubo);

  if (mesh_shader_create_standard(mesh, MeshShader_Stencil) ==
      MeshStatus_Success)
    mesh_shader_build_mvp(mesh, MeshShader_Stencil, scene->ubo);
}

/**
   Build meshes Wireframe shader in each scene list
   Establish pipeline from previously set bind groups
 */
void engine_build_mesh_wireframe(Scene *scene, Mesh *mesh) {

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

    mesh_shader_build_mvp(mesh, MeshShader_Wireframe, scene->ubo);
  }
}

/**
   Build meshes Boundbox shader in each scene list
   Establish pipeline from previously set bind groups
 */
void engine_build_mesh_boundbox(Scene *scene, Mesh *mesh) {

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

    mesh_shader_build_mvp(mesh, MeshShader_Wireframe, scene->ubo);
  }
}
