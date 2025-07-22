#include "build.h"
#include "../material/material.h"
#include "../utils/system.h"
#include "core.h"
#include <stdint.h>

// pipeline builders
static void scene_build_mesh_texture(Mesh *, Camera *, Viewport *,
                                     PipelineMultisampleCount, LightList *);

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

  SceneRendererDrawMode draw_mode = scene->renderer.draw_mode;
  PipelineMultisampleCount sample_count = scene->renderer.multisampling.count;
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
    // Build boundbox & wireframe by default for selection
    scene_build_mesh_boundbox(mesh, camera, viewport, sample_count);

    // Dynamic rendering
    switch (draw_mode) {

    case SceneRendererDrawMode_Solid:
      VERBOSE_MESH_BUILD("Solid %s", mesh->name);
      scene_build_mesh_solid(mesh, camera, viewport,
                             sample_count); // build solid
      break;

    case SceneRendererDrawMode_Wireframe:
      VERBOSE_MESH_BUILD("Wireframe %s", mesh->name);
      scene_build_mesh_wireframe(mesh, camera, viewport,
                                 sample_count); // build wireframe
      break;

    case SceneRendererDrawMode_Boundbox:
      VERBOSE_MESH_BUILD("Boundbox %s", mesh->name);
      scene_build_mesh_boundbox(mesh, camera, viewport,
                                sample_count); // build boundbox
      break;

    case SceneRendererDrawMode_Texture:
      VERBOSE_MESH_BUILD("Texture %s", mesh->name);

      // Bake AO textures for static scenes elements
      ao_bake_init(&(AOBakeInitDescriptor){
          .queue = queue,
          .device = device,
          .mesh_list = &scene->pipelines[ScenePipeline_Lit],
      });

      // Setup drawing pass may need to move it elsewhere
      shadow_pass_init(&(ShadowMapInitDescriptor){
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

      LightList *lights =
          (pipeline == ScenePipeline_Lit) ? &scene->lights : NULL;
      scene_build_mesh_texture(mesh, camera, viewport, sample_count, lights);
      break;

    default:
      break;
    }
    break;
  }
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
                              LightList *lights) {

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                       &mesh->topology.boundbox);

  // bind views
  material_texture_bind_views(mesh, camera, viewport,
                              SHADER_TEXTURE_BINDGROUP_VIEWS);

  // bind lights (only if provided)
  if (lights != NULL)
    material_texture_bind_lights(mesh, lights, SHADER_TEXTURE_BINDGROUP_LIGHTS);

  // build layer
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

  // build solid meshes first
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

  // build "solid" meshes first
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

  // build "solid" meshes first
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

  // updating meshes shader's pipeline sampling (dirty)
  pipeline_set_sampling(shader_pipeline(target_shader(mesh)), sample);

  // build shader pipeline
  mesh_build(mesh, target_shader(mesh));

  // release shader module
  shader_module_release(target_shader(mesh));
}
