#include "build.h"
#include "../runtime/mesh/shader/shader.h"
#include "../utils/system.h"
#include "core.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

typedef struct {
  ScenePipeline pipeline;
  LightList *lights;
  WGPUTextureView point_map;
  WGPUTextureView spot_map;
} SceneBuildTextureDescriptor;

typedef struct {
  Mesh *mesh;
  SSBOManager *ssbo;
  UBOManager *ubo;
} SceneBuildDescriptor;

// pipeline builders
static inline void
scene_build_mesh_texture(const SceneBuildDescriptor *,
                         const SceneBuildTextureDescriptor *);

static inline void scene_build_mesh_solid(const SceneBuildDescriptor *);
static inline void scene_build_mesh_wireframe(const SceneBuildDescriptor *);
static inline void scene_build_mesh_fixed(const SceneBuildDescriptor *);
static inline void scene_build_mesh_boundbox(const SceneBuildDescriptor *);

/**
   ▗▄▄▄ ▗▄▄▄▖ ▗▄▄▖▗▄▄▖  ▗▄▖▗▄▄▄▖▗▄▄▖▗▖ ▗▖▗▄▄▄▖▗▄▄▖
   ▐▌  █  █  ▐▌   ▐▌ ▐▌▐▌ ▐▌ █ ▐▌   ▐▌ ▐▌▐▌   ▐▌ ▐▌
   ▐▌  █  █   ▝▀▚▖▐▛▀▘ ▐▛▀▜▌ █ ▐▌   ▐▛▀▜▌▐▛▀▀▘▐▛▀▚▖
   ▐▙▄▄▀▗▄█▄▖▗▄▄▞▘▐▌   ▐▌ ▐▌ █ ▝▚▄▄▖▐▌ ▐▌▐▙▄▄▖▐▌ ▐▌

   Build the mesh according to the current scene rendere mode and pipeline in
   which the mesh will be added to.
 */
void scene_build_mesh(Scene *scene, Mesh *mesh, const ScenePipeline pipeline) {

  Camera *camera = scene->active_camera;
  Viewport *viewport = &scene->viewport;

  SSBOManager *ssbo = &scene->renderer.ssbo;
  UBOManager *ubo = &scene->renderer.ubo;

  const SceneRendererDrawMode draw_mode = scene->renderer.draw.mode;
  const WGPUQueue queue = scene_queue(scene);
  const WGPUDevice device = scene_device(scene);

  // bind new mesh uniform to SSBO and copy previous mesh uniform data
  ssbo_copy_entry(ssbo, SSBOType_Mesh, &mesh->ssbo_slot);

  SceneBuildDescriptor build_desc = {.mesh = mesh, .ssbo = ssbo, .ubo = ubo};

  switch (pipeline) {

    // Fixed rendering (NOT part of shader/topology creation automation, meaning
    // it's the developer responsibility to create the relative topology and
    // shaders.)
  case ScenePipeline_Fixed:
  case ScenePipeline_Fixed_Selection:
  case ScenePipeline_Fixed_Front:
  case ScenePipeline_Fixed_Background:
#ifdef VERBOSE_BUILDING_PHASE
    VERBOSE_MESH_BUILD("Fixed %s", mesh->name);
#endif
    scene_build_mesh_fixed(&build_desc);
    break;

  default:

// EDITORONLY
// Build Boundbox & Wireframe by default for selection
#ifdef VERBOSE_BUILDING_PHASE
    VERBOSE_MESH_BUILD("Boundbox %s", mesh->name);
#endif
    scene_build_mesh_boundbox(&build_desc);

    // Dynamic rendering
    switch (draw_mode) {

      // build boundbox
    case SceneRendererDrawMode_Boundbox:
    default:
      break;

      // build solid
    case SceneRendererDrawMode_Solid:
#ifdef VERBOSE_BUILDING_PHASE
      VERBOSE_MESH_BUILD("Solid %s", mesh->name);
#endif
      scene_build_mesh_solid(&build_desc);
      break;

      // build wireframe
    case SceneRendererDrawMode_Wireframe:
#ifdef VERBOSE_BUILDING_PHASE
      VERBOSE_MESH_BUILD("Wireframe %s", mesh->name);
#endif
      scene_build_mesh_wireframe(&build_desc);
      break;

    // build texture
    // Since build texture include AO baking and shadow mapping we need to enter
    // more arguments compared to the other builds
    case SceneRendererDrawMode_Texture:
#ifdef VERBOSE_BUILDING_PHASE
      VERBOSE_MESH_BUILD("Texture %s", mesh->name);
#endif

      scene_build_mesh_texture(
          &build_desc,
          &(SceneBuildTextureDescriptor){
              .pipeline = pipeline,
              .lights = &scene->lights,
              .point_map =
                  scene->lights.point.shadow.pass.depth.attachment.view,
              .spot_map = scene->lights.spot.shadow.pass.depth.attachment.view,
          });

      break;
    }
    break;
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
void scene_build_mesh_texture(
    const SceneBuildDescriptor *build_desc,
    const SceneBuildTextureDescriptor *build_texture_desc) {

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&build_desc->mesh->topology.base,
                                       build_desc->mesh->model,
                                       &build_desc->mesh->topology.boundbox);

  // bind views
  mesh_shader_build_mvp(build_desc->mesh, MeshShader_Texture, build_desc->ssbo);

  mesh_shader_build_mvp(build_desc->mesh, MeshShader_Reflection,
                        build_desc->ssbo);

  // lit and shadow pipeline
  if (build_texture_desc->pipeline &
      (ScenePipeline_Dynamic_LitShadow | ScenePipeline_Dynamic_Lit)) {

    // bind lights
    mesh_shader_texture_update_lights(build_desc->mesh, MeshShader_Texture,
                                      build_desc->ubo, build_desc->ssbo);

    mesh_shader_texture_update_lights(build_desc->mesh, MeshShader_Reflection,
                                      build_desc->ubo, build_desc->ssbo);
  }

  // shadow only pipeline
  if (build_texture_desc->pipeline == ScenePipeline_Dynamic_LitShadow) {

    // create binding for shadow maps (using fallback texture)
    mesh_shader_texture_bind_shadow_maps(build_desc->mesh,
                                         build_texture_desc->point_map,
                                         build_texture_desc->spot_map);

    // create mesh shadow shader
    mesh_shader_create_shadow(build_desc->mesh);

    // bind light and mesh uniform to shadow
    mesh_shader_build_mp(build_desc->mesh, MeshShader_Shadow, build_desc->ssbo,
                         SSBOType_ViewProjection);
  }

  // set active shader
  mesh_shader_set_active(build_desc->mesh, MeshShader_Texture);
}

/**
   Build meshes Solid shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_mesh_solid(const SceneBuildDescriptor *build_desc) {

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&build_desc->mesh->topology.base,
                                       build_desc->mesh->model,
                                       &build_desc->mesh->topology.boundbox);

  // create meshes' solid shader
  mesh_shader_create_solid(build_desc->mesh);

  // bind views
  mesh_shader_build_mvp(build_desc->mesh, MeshShader_Solid, build_desc->ssbo);

  // set active shader
  mesh_shader_set_active(build_desc->mesh, MeshShader_Solid);
}

/**
   Build meshes Wireframe shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_mesh_wireframe(const SceneBuildDescriptor *build_desc) {

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&build_desc->mesh->topology.base,
                                       build_desc->mesh->model,
                                       &build_desc->mesh->topology.boundbox);

  // create wireframe topology
  MeshTopology src_topo =
      mesh_topology_base_vertex(&build_desc->mesh->topology.base);
  MeshTopologyWireframe *dest_topo = &build_desc->mesh->topology.wireframe;
  mesh_topology_wireframe_create(&src_topo, dest_topo, build_desc->mesh->device,
                                 build_desc->mesh->queue);

  // create meshes' wireframe shader
  if (mesh_shader_create_wireframe(build_desc->mesh) == MeshStatus_Success)
    mesh_shader_build_mvp(build_desc->mesh, MeshShader_Wireframe,
                          build_desc->ssbo);

  // set active shader
  mesh_shader_set_active(build_desc->mesh, MeshShader_Wireframe);
}

/**
   Build meshes Boundbox shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_mesh_boundbox(const SceneBuildDescriptor *build_desc) {

  // create full boundbox topology
  MeshTopologyBoundbox *dest_topo = &build_desc->mesh->topology.boundbox;
  mesh_topology_boundbox_create(
      &build_desc->mesh->topology.base, build_desc->mesh->model, dest_topo,
      build_desc->mesh->device, build_desc->mesh->queue);

  // create meshes' wireframe shader
  mesh_shader_create_wireframe(build_desc->mesh);

  // bind views
  mesh_shader_build_mvp(build_desc->mesh, MeshShader_Wireframe,
                        build_desc->ssbo);

  // set active shader
  mesh_shader_set_active(build_desc->mesh, MeshShader_Wireframe);
}

/**
   Build Fixed mesh layer.
   Fixed layer use the Override shader* as default shader.
 */
void scene_build_mesh_fixed(const SceneBuildDescriptor *build_desc) {

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&build_desc->mesh->topology.base,
                                       build_desc->mesh->model,
                                       &build_desc->mesh->topology.boundbox);

  // bind views
  mesh_shader_build_mvp(build_desc->mesh, MeshShader_Fixed, build_desc->ssbo);

  // set active shader
  mesh_shader_set_active(build_desc->mesh, MeshShader_Fixed);
}
