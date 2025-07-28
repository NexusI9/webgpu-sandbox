#include "build.h"
#include "../material/material.h"
#include "../utils/system.h"
#include "core.h"
#include "webgpu/webgpu.h"
#include <stdint.h>

typedef struct {
  ScenePipeline pipeline;
  LightList *lights;
  WGPUTextureView *point_map;
  WGPUTextureView *spot_map;
} SceneBuildTextureDescriptor;

// pipeline builders
static void scene_build_mesh_texture(Mesh *, Camera *, Viewport *,
                                     PipelineMultisampleCount,
                                     const SceneBuildTextureDescriptor *,
                                     const AOBakeInitDescriptor *,
                                     const ShadowMapInitDescriptor *);

static void scene_build_mesh_shadow(Mesh *, Camera *, Viewport *,
                                    PipelineMultisampleCount);

static void scene_build_mesh_solid(Mesh *, Camera *, Viewport *,
                                   PipelineMultisampleCount);

static void scene_build_mesh_wireframe(Mesh *, Camera *, Viewport *,
                                       PipelineMultisampleCount);

static void scene_build_mesh_fixed(Mesh *, Camera *, Viewport *,
                                   PipelineMultisampleCount);

static void scene_build_mesh_boundbox(Mesh *, Camera *, Viewport *,
                                      PipelineMultisampleCount);

// utils
static inline void build_utils_bind(Mesh *, mesh_get_shader_callback,
                                    PipelineMultisampleCount);

/**
   ▗▄▄▄ ▗▄▄▄▖ ▗▄▄▖▗▄▄▖  ▗▄▖▗▄▄▄▖▗▄▄▖▗▖ ▗▖▗▄▄▄▖▗▄▄▖
   ▐▌  █  █  ▐▌   ▐▌ ▐▌▐▌ ▐▌ █ ▐▌   ▐▌ ▐▌▐▌   ▐▌ ▐▌
   ▐▌  █  █   ▝▀▚▖▐▛▀▘ ▐▛▀▜▌ █ ▐▌   ▐▛▀▜▌▐▛▀▀▘▐▛▀▚▖
   ▐▙▄▄▀▗▄█▄▖▗▄▄▞▘▐▌   ▐▌ ▐▌ █ ▝▚▄▄▖▐▌ ▐▌▐▙▄▄▖▐▌ ▐▌

   Build the mesh according to the current scene rendere mode and pipeline in
   which the mesh will be added to.
 */
void scene_build_mesh(Scene *scene, Mesh *mesh, const ScenePipeline pipeline) {

  SceneRendererDrawMode draw_mode = scene->renderer.draw.mode;
  PipelineMultisampleCount sample_count = scene->renderer.texture.multisample;
  Camera *camera = scene->active_camera;
  Viewport *viewport = &scene->viewport;
  WGPUQueue *queue = scene_queue(scene);
  WGPUDevice *device = scene_device(scene);

  switch (pipeline) {

  case ScenePipeline_Fixed:
    // Fixed rendering (NOT part of shader/topology creation automation, meaning
    // it's the developer responsibility to create the relative topology and
    // shaders.)
    VERBOSE_MESH_BUILD("Fixed %s", mesh->name);
    scene_build_mesh_fixed(mesh, camera, viewport, sample_count);
    break;

  default:

    // EDITORONLY
    // Build Boundbox & Wireframe by default for selection
    VERBOSE_MESH_BUILD("Boundbox %s", mesh->name);
    scene_build_mesh_boundbox(mesh, camera, viewport, sample_count);

    // Dynamic rendering
    switch (draw_mode) {

      // build boundbox
    case SceneRendererDrawMode_Boundbox:
    default:
      break;

      // build solid
    case SceneRendererDrawMode_Solid:
      VERBOSE_MESH_BUILD("Solid %s", mesh->name);
      scene_build_mesh_solid(mesh, camera, viewport, sample_count);
      break;

      // build wireframe
    case SceneRendererDrawMode_Wireframe:
      VERBOSE_MESH_BUILD("Wireframe %s", mesh->name);
      scene_build_mesh_wireframe(mesh, camera, viewport, sample_count);
      break;

    // build texture
    // Since build texture include AO baking and shadow mapping we need to enter
    // more arguments compared to the other builds
    case SceneRendererDrawMode_Texture:
      VERBOSE_MESH_BUILD("Texture %s", mesh->name);

      scene_build_mesh_texture(
          mesh, camera, viewport, sample_count,
          &(SceneBuildTextureDescriptor){
              .pipeline = pipeline,
              .lights = &scene->lights,
              .point_map =
                  &scene->renderer.texture.fallback.depth_cube_array_view,
              .spot_map = &scene->renderer.texture.fallback.depth_2d_array_view,
          },
          &(AOBakeInitDescriptor){
              .queue = queue,
              .device = device,
              .mesh_list = &scene->pipelines[ScenePipeline_Lit],
          },
          &(ShadowMapInitDescriptor){
              .device = device,
              .queue = queue,
              .mesh_list = &scene->pipelines[ScenePipeline_Lit],
              .lights =
                  {
                      .point = &scene->lights.point,
                      .sun = &scene->lights.sun,
                      .spot = &scene->lights.spot,
                  },

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
                              PipelineMultisampleCount sample,
                              const SceneBuildTextureDescriptor *build_desc,
                              const AOBakeInitDescriptor *ao_desc,
                              const ShadowMapInitDescriptor *shad_desc) {

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                       &mesh->topology.boundbox);

  // bind views
  material_texture_bind_views(mesh, camera, viewport,
                              SHADER_TEXTURE_BINDGROUP_VIEWS);

  // lit only pipeline
  if (build_desc->pipeline == ScenePipeline_Lit) {
    // create binding for shadow maps (using fallback texture)
    material_texture_bind_shadow_maps(mesh, *build_desc->point_map,
                                      *build_desc->spot_map);

    // bind lights
    material_texture_bind_lights(mesh, build_desc->lights,
                                 SHADER_TEXTURE_BINDGROUP_LIGHTS);

    // Bake AO textures for static scenes elements
    ao_bake_init(ao_desc);

    // Setup drawing pass may need to move it elsewhere
    shadow_pass_init(shad_desc);
  }

  // build mesh
  build_utils_bind(mesh, mesh_shader_texture, sample);
}

/**
   Build meshes Solid shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_mesh_solid(Mesh *mesh, Camera *camera, Viewport *viewport,
                            PipelineMultisampleCount sample) {

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                       &mesh->topology.boundbox);

  // create meshes' solid shader
  mesh_create_solid_shader(mesh);

  // bind views
  material_solid_bind_views(mesh, camera, viewport,
                            SHADER_SOLID_BINDGROUP_VIEWS);

  build_utils_bind(mesh, mesh_shader_solid, sample);
}

/**
   Build meshes Wireframe shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_mesh_wireframe(Mesh *mesh, Camera *camera, Viewport *viewport,
                                PipelineMultisampleCount sample) {

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                       &mesh->topology.boundbox);

  // create wireframe topology
  MeshTopology src_topo = mesh_topology_base_vertex(&mesh->topology.base);
  MeshTopologyWireframe *dest_topo = &mesh->topology.wireframe;
  mesh_topology_wireframe_create(&src_topo, dest_topo, mesh->device,
                                 mesh->queue);

  // create meshes' wireframe shader
  mesh_create_wireframe_shader(mesh);

  // bind views
  material_wireframe_bind_views(mesh, camera, viewport,
                                SHADER_WIREFRAME_BINDGROUP_VIEWS);

  // already built during the defaut boundbox
  build_utils_bind(mesh, mesh_shader_wireframe, sample);
}

/**
   Build meshes Boundbox shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_mesh_boundbox(Mesh *mesh, Camera *camera, Viewport *viewport,
                               PipelineMultisampleCount sample) {

  // create full boundbox topology
  MeshTopologyBoundbox *dest_topo = &mesh->topology.boundbox;
  mesh_topology_boundbox_create(&mesh->topology.base, mesh->model, dest_topo,
                                mesh->device, mesh->queue);

  // create meshes' wireframe shader
  mesh_create_wireframe_shader(mesh);

  // bind views
  material_wireframe_bind_views(mesh, camera, viewport,
                                SHADER_WIREFRAME_BINDGROUP_VIEWS);

  build_utils_bind(mesh, mesh_shader_wireframe, sample);
}

/**
   Build meshes Shadow shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_build_mesh_shadow(Mesh *mesh, Camera *camera, Viewport *viewport,
                             PipelineMultisampleCount sample) {

  // (shadow is bind process already during the renderer shadow pass)

  build_utils_bind(mesh, mesh_shader_shadow, sample);
}

/**
   Build Fixed mesh layer.
   Fixed layer use the Override shader* as default shader.
 */
void scene_build_mesh_fixed(Mesh *mesh, Camera *camera, Viewport *viewport,
                            PipelineMultisampleCount sample) {

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                       &mesh->topology.boundbox);

  // bind views
  material_override_bind_views(mesh, camera, viewport,
                               SHADER_FIXED_BINDGROUP_VIEWS);

  // build fixed
  build_utils_bind(mesh, mesh_shader_override, sample);
}

/**
    ▗▖ ▗▖▗▄▄▄▖▗▄▄▄▖▗▖    ▗▄▄▖
    ▐▌ ▐▌  █    █  ▐▌   ▐▌
    ▐▌ ▐▌  █    █  ▐▌    ▝▀▚▖
    ▝▚▄▞▘  █  ▗▄█▄▖▐▙▄▄▖▗▄▄▞▘

 */

void build_utils_bind(Mesh *mesh, mesh_get_shader_callback target_shader,
                      PipelineMultisampleCount sample) {

  Shader *shader = target_shader(mesh);
  // abort build if already built
  if (shader_is_built(shader))
    return;

  // updating meshes shader's pipeline sampling (dirty)
  pipeline_set_sampling(shader_pipeline(target_shader(mesh)), sample);

  // build shader pipeline
  mesh_build(mesh, target_shader(mesh));

  // release shader module
  shader_module_release(target_shader(mesh));
}
