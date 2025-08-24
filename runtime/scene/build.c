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

// pipeline builders
static void scene_build_mesh_texture(Mesh *, Camera *, Viewport *,
                                     const SceneBuildTextureDescriptor *);

static void scene_build_mesh_solid(Mesh *, Camera *, Viewport *);

static void scene_build_mesh_wireframe(Mesh *, Camera *, Viewport *);

static void scene_build_mesh_fixed(Mesh *, Camera *, Viewport *);

static void scene_build_mesh_boundbox(Mesh *, Camera *, Viewport *);

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

  const SceneRendererDrawMode draw_mode = scene->renderer.draw.mode;
  const WGPUQueue queue = scene_queue(scene);
  const WGPUDevice device = scene_device(scene);

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
    scene_build_mesh_fixed(mesh, camera, viewport);
    break;

  default:

// EDITORONLY
// Build Boundbox & Wireframe by default for selection
#ifdef VERBOSE_BUILDING_PHASE
    VERBOSE_MESH_BUILD("Boundbox %s", mesh->name);
#endif
    scene_build_mesh_boundbox(mesh, camera, viewport);

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
      scene_build_mesh_solid(mesh, camera, viewport);
      break;

      // build wireframe
    case SceneRendererDrawMode_Wireframe:
#ifdef VERBOSE_BUILDING_PHASE
      VERBOSE_MESH_BUILD("Wireframe %s", mesh->name);
#endif
      scene_build_mesh_wireframe(mesh, camera, viewport);
      break;

    // build texture
    // Since build texture include AO baking and shadow mapping we need to enter
    // more arguments compared to the other builds
    case SceneRendererDrawMode_Texture:
#ifdef VERBOSE_BUILDING_PHASE
      VERBOSE_MESH_BUILD("Texture %s", mesh->name);
#endif

      scene_build_mesh_texture(
          mesh, camera, viewport,
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
void scene_build_mesh_texture(Mesh *mesh, Camera *camera, Viewport *viewport,
                              const SceneBuildTextureDescriptor *build_desc) {

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                       &mesh->topology.boundbox);

  // bind views
  mesh_shader_build_mvp(mesh, MeshShader_Texture, camera, viewport, true);
  mesh_shader_build_mvp(mesh, MeshShader_Reflection, camera, viewport, false);

  // lit and shadow pipeline
  if (build_desc->pipeline &
      (ScenePipeline_Dynamic_LitShadow | ScenePipeline_Dynamic_Lit)) {

    // bind lights
    mesh_shader_texture_update_lights(mesh, build_desc->lights,
                                      SHADER_TEXTURE_BINDGROUP_LIGHTS);
  }

  // shadow only pipeline
  if (build_desc->pipeline == ScenePipeline_Dynamic_LitShadow) {

    // create binding for shadow maps (using fallback texture)
    mesh_shader_texture_bind_shadow_maps(mesh, build_desc->point_map,
                                         build_desc->spot_map);

    // create mesh shadow shader
    mesh_shader_create_shadow(mesh);

    // bind light and mesh uniform to shadow
    mesh_shader_shadow_update_mvp(mesh);
  }

  // set active shader
  mesh_shader_set_active(mesh, MeshShader_Texture);
}

/**
   Build meshes Solid shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_mesh_solid(Mesh *mesh, Camera *camera, Viewport *viewport) {

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                       &mesh->topology.boundbox);

  // create meshes' solid shader
  mesh_shader_create_solid(mesh);

  // bind views
  mesh_shader_build_mvp(mesh, MeshShader_Solid, camera, viewport, true);

  // set active shader
  mesh_shader_set_active(mesh, MeshShader_Solid);
}

/**
   Build meshes Wireframe shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_mesh_wireframe(Mesh *mesh, Camera *camera,
                                Viewport *viewport) {

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                       &mesh->topology.boundbox);

  // create wireframe topology
  MeshTopology src_topo = mesh_topology_base_vertex(&mesh->topology.base);
  MeshTopologyWireframe *dest_topo = &mesh->topology.wireframe;
  mesh_topology_wireframe_create(&src_topo, dest_topo, mesh->device,
                                 mesh->queue);

  // create meshes' wireframe shader
  mesh_shader_create_wireframe(mesh);

  // bind views
  mesh_shader_build_mvp(mesh, MeshShader_Wireframe, camera, viewport, true);

  // set active shader
  mesh_shader_set_active(mesh, MeshShader_Wireframe);
}

/**
   Build meshes Boundbox shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_mesh_boundbox(Mesh *mesh, Camera *camera, Viewport *viewport) {

  // create full boundbox topology
  MeshTopologyBoundbox *dest_topo = &mesh->topology.boundbox;
  mesh_topology_boundbox_create(&mesh->topology.base, mesh->model, dest_topo,
                                mesh->device, mesh->queue);

  // create meshes' wireframe shader
  mesh_shader_create_wireframe(mesh);

  // bind views
  mesh_shader_build_mvp(mesh, MeshShader_Wireframe, camera, viewport, true);

  // set active shader
  mesh_shader_set_active(mesh, MeshShader_Wireframe);
}

/**
   Build Fixed mesh layer.
   Fixed layer use the Override shader* as default shader.
 */
void scene_build_mesh_fixed(Mesh *mesh, Camera *camera, Viewport *viewport) {

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                       &mesh->topology.boundbox);

  // bind views
  mesh_shader_build_mvp(mesh, MeshShader_Fixed, camera, viewport, true);

  // set active shader
  mesh_shader_set_active(mesh, MeshShader_Fixed);
}
