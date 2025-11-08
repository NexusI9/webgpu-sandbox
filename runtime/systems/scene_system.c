#include "scene_system.h"
#include "backend/renderer/core.h"
#include "backend/renderer/reflection/draw.h"
#include "backend/renderer/render_pass/texture.h"
#include "backend/renderer/shadow_map/draw.h"
#include "backend/resource_manager.h"
#include "runtime/geometry/line/core.h"
#include "runtime/light/core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/mesh/shader/texture.h"
#include "runtime/pipeline/render.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor_mesh/grid/grid.h"
#include "runtime/scene/stat.h"
#include "utils/color.h"

/**


   ▗▄▄▄ ▗▄▄▖  ▗▄▖ ▗▖ ▗▖
   ▐▌  █▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌
   ▐▌  █▐▛▀▚▖▐▛▀▜▌▐▌ ▐▌
   ▐▙▄▄▀▐▌ ▐▌▐▌ ▐▌▐▙█▟▌



 */
void scene_system_set_draw_mode(Scene *scene, Renderer *renderer,
                                const RendererDrawMode mode) {

  if (mode == renderer->draw_mode)
    return;

  profiler_latency_clear_all(&renderer->profiler);

  // update light / reflections
  if (mode == RendererDrawMode_Texture) {

    renderer_draw_shadow_map_all(
        &(ShadowMapDrawAllDescriptor){
            .mesh_list =
                renderer_pipeline(renderer, RendererPipeline_Dynamic_LitShadow),
            .lights = &scene->lights,
            .profiler = &renderer->profiler},
        SCENE_DEBUG_UNDEFINED);
  }

  // update renderer drawn render pass configuration
  renderer_set_draw_mode(renderer, mode);
}

/**

    ▗▖  ▗▖▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖▗▄▄▖ ▗▄▄▄▖▗▖   ▗▄▄▄▖▗▄▄▄▖▗▖  ▗▖
    ▐▌  ▐▌  █  ▐▌     █  ▐▌ ▐▌  █  ▐▌     █    █   ▝▚▞▘
    ▐▌  ▐▌  █   ▝▀▚▖  █  ▐▛▀▚▖  █  ▐▌     █    █    ▐▌
     ▝▚▞▘ ▗▄█▄▖▗▄▄▞▘▗▄█▄▖▐▙▄▞▘▗▄█▄▖▐▙▄▄▖▗▄█▄▖  █    ▐▌


 */
void scene_system_show_mesh(Scene *scene, Renderer *renderer, Mesh *mesh) {
  renderer_show_mesh(renderer, mesh);
  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}

void scene_system_hide_mesh(Scene *scene, Renderer *renderer, Mesh *mesh) {
  renderer_hide_mesh(renderer, mesh);
  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}

void scene_system_show_mesh_ref_list(Scene *scene, Renderer *renderer,
                                     MeshRefList *list) {
  renderer_show_mesh_ref_list(renderer, list);
  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}
void scene_system_hide_mesh_ref_list(Scene *scene, Renderer *renderer,
                                     MeshRefList *list) {
  renderer_hide_mesh_ref_list(renderer, list);
  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}

void scene_system_toggle_mesh_visibility(Scene *scene, Renderer *renderer,
                                         Mesh *mesh) {
  renderer_visibility_toggle_mesh(renderer, mesh);
  scene_system_update_vertex_count(scene, renderer);
  scene_system_update_draw_call_count(scene, renderer);
}

/*

   ▗▄▄▖ ▗▖ ▗▖▗▄▄▄▖▗▖   ▗▄▄▄
   ▐▌ ▐▌▐▌ ▐▌  █  ▐▌   ▐▌  █
   ▐▛▀▚▖▐▌ ▐▌  █  ▐▌   ▐▌  █
   ▐▙▄▞▘▝▚▄▞▘▗▄█▄▖▐▙▄▄▖▐▙▄▄▀


   Mesh Building process:

   The scene building process handles each layers respective essentials shader
   creation or binding process( view matrix...).

   Currently the renderer handles different passes such as :
   - Topology Creation
   - Shader creation
   - Shader bind views
   - Shader bind lights
   - Shader build pipeline layout

   .---------------------------------------------------------------------.
   |                ADD               |              REMOVE              |
   |---------------------------------------------------------------------|
   |   Add and Remove functions basically mounts, unmounts the mesh      |
   |   from the scene. Meaning they build the mesh shader internally     |
   |   and show it visually by adding it to the pipeline list.           |
   |   Those 2 functions should only be used for first and last instan-  |
   |   tiation of the mesh.                                              |
   |                                                                     |
   |   .------------ ⚙ ------------.    .------------ ◉ -------------.  |
   |   |    BUILD    |   UNBUILD    |    |     SHOW    |     HIDE     |  |
   |   |----------------------------|    |----------------------------|  |
   |   | Build and Unbuild function | => | Show and Hide functions    |  |
   |   | only handle the mesh       | => | operate at a visual level  |  |
   |   | internal binding. It does  | => | only. They only pop or push|  |
   |   | not visually add the mesh  | => | the mesh from the pipeline |  |
   |   | to the scene pipeline.     | => | array. However it's        |  |
   |   | Building only "prepares"   | => | important to make sure the |  |
   |   | the mesh for the drawcall. |    | mesh is Built priorly.     |  |
   |   '----------------------------'    '----------------------------'  |
   '---------------------------------------------------------------------'

 */

static inline SceneStatus scene_system_build_mesh(Scene *, Mesh *,
                                                  const RendererPipeline);
static inline void scene_system_build_mesh_ref_list(Scene *, MeshRefList *,
                                                    const RendererPipeline);

typedef void (*scene_system_builder_callback)(Scene *, Mesh *,
                                              const RenderPipeline *);

// pipeline builders
static inline void scene_system_build_mesh_texture(Scene *, Mesh *,
                                                   const RendererPipeline);

static inline void scene_system_build_mesh_solid(Scene *, Mesh *,
                                                 const RendererPipeline);

static inline void scene_system_build_mesh_wireframe(Scene *, Mesh *,
                                                     const RendererPipeline);

static inline void scene_system_build_mesh_outline(Scene *, Mesh *,
                                                   const RendererPipeline);

static inline void scene_system_build_mesh_fixed(Scene *, Mesh *,
                                                 const RendererPipeline);

static inline void scene_system_build_mesh_boundbox(Scene *, Mesh *,
                                                    const RendererPipeline);

/**
   ▗▄▄▄ ▗▄▄▄▖ ▗▄▄▖▗▄▄▖  ▗▄▖▗▄▄▄▖▗▄▄▖▗▖ ▗▖▗▄▄▄▖▗▄▄▖
   ▐▌  █  █  ▐▌   ▐▌ ▐▌▐▌ ▐▌ █ ▐▌   ▐▌ ▐▌▐▌   ▐▌ ▐▌
   ▐▌  █  █   ▝▀▚▖▐▛▀▘ ▐▛▀▜▌ █ ▐▌   ▐▛▀▜▌▐▛▀▀▘▐▛▀▚▖
   ▐▙▄▄▀▗▄█▄▖▗▄▄▞▘▐▌   ▐▌ ▐▌ █ ▝▚▄▄▖▐▌ ▐▌▐▙▄▄▖▐▌ ▐▌

   Build the mesh according to the current scene rendere mode and pipeline in
   which the mesh will be added to.
 */
SceneStatus scene_system_build_mesh(Scene *scene, Mesh *mesh,
                                    const RendererPipeline pipeline) {

  UBOManager *ubo = scene->ubo;

  if (pipeline >= RendererPipeline_Fixed_Background) {
    /*
      === Fixed rendering ===

     (NOT part of shader/topology creation automation, meaning
     it's the developer responsibility to create the relative topology and
     shaders.)
    */

    scene_system_build_mesh_fixed(scene, mesh, pipeline);

  } else {

    {
      // EDITORONLY
      scene_system_build_mesh_outline(scene, mesh, pipeline);
    }

    {
      scene_system_build_mesh_boundbox(scene, mesh, pipeline);
      scene_system_build_mesh_solid(scene, mesh, pipeline);
      scene_system_build_mesh_wireframe(scene, mesh, pipeline);
      scene_system_build_mesh_texture(scene, mesh, pipeline);
    }
  }

  return SceneStatus_Success;
}

void scene_system_build_mesh_ref_list(Scene *scene, MeshRefList *list,
                                      const RendererPipeline pipeline) {
  for (size_t i = 0; i < list->length; i++)
    scene_system_build_mesh(scene, list->entries[i], pipeline);
}

/**
  ▗▄▄▖ ▗▖ ▗▖▗▄▄▄▖▗▖   ▗▄▄▄ ▗▄▄▄▖▗▄▄▖  ▗▄▄▖
  ▐▌ ▐▌▐▌ ▐▌  █  ▐▌   ▐▌  █▐▌   ▐▌ ▐▌▐▌
  ▐▛▀▚▖▐▌ ▐▌  █  ▐▌   ▐▌  █▐▛▀▀▘▐▛▀▚▖ ▝▀▚▖
  ▐▙▄▞▘▝▚▄▞▘▗▄█▄▖▐▙▄▄▖▐▙▄▄▀▐▙▄▄▖▐▌ ▐▌▗▄▄▞▘

   Build meshes Texture shader in each scene list
   Establish pipeline from previously set bind groups
 */
void scene_system_build_mesh_texture(Scene *scene, Mesh *mesh,
                                     const RendererPipeline pipeline) {

#ifdef VERBOSE_BUILDING_PHASE
  logger_add(LoggerFlag_MeshBuild, "Texture %s", mesh->name);
#endif

  UBOManager *ubo = scene->ubo;

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                       &mesh->topology.boundbox);

  // bind views
  mesh_shader_build_mvp(mesh, MeshShader_Texture, ubo);
  mesh_shader_build_mvp(mesh, MeshShader_Reflection, ubo);

  if (pipeline &
      (RendererPipeline_Dynamic_Unlit | RendererPipeline_Dynamic_LitAlpha |
       RendererPipeline_Dynamic_LitShadow | RendererPipeline_Dynamic_Lit)) {

    mesh_shader_texture_update_environment(
        mesh, scene->environment.skybox.view,
        (SceneEnvironmentUniform *)scene->environment.ubo_slot.uniform, ubo);

    mesh_shader_texture_update_probes(
        mesh, scene->probes.reflection_plane.pass.color.attachment.view,
        scene->probes.reflection_probe.pass.color.attachment.view, ubo);
  }

  if (pipeline &
      (RendererPipeline_Dynamic_LitShadow | RendererPipeline_Dynamic_Lit |
       RendererPipeline_Dynamic_LitAlpha)) {

    mesh_shader_texture_update_lights(mesh, MeshShader_Texture, ubo);
    mesh_shader_texture_update_lights(mesh, MeshShader_Reflection, ubo);
  }

  if (pipeline & (RendererPipeline_Dynamic_LitShadow |
                  RendererPipeline_Dynamic_LitAlpha)) {

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
void scene_system_build_mesh_solid(Scene *scene, Mesh *mesh,
                                   const RendererPipeline pipeline) {

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
void scene_system_build_mesh_outline(Scene *scene, Mesh *mesh,
                                     const RendererPipeline pipeline) {

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
void scene_system_build_mesh_wireframe(Scene *scene, Mesh *mesh,
                                       const RendererPipeline pipeline) {

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
void scene_system_build_mesh_boundbox(Scene *scene, Mesh *mesh,
                                      const RendererPipeline pipeline) {

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

/**
   Build Fixed mesh layer.
   Fixed layer use the Override shader* as default shader.
 */
void scene_system_build_mesh_fixed(Scene *scene, Mesh *mesh,
                                   const RendererPipeline pipeline) {

#ifdef VERBOSE_BUILDING_PHASE
  logger_add(LoggerFlag_MeshBuild, "Fixed %s", mesh->name);
#endif

  // compute boundbox bounds for collisions (lightweight)
  mesh_topology_boundbox_compute_bound(&mesh->topology.base, mesh->model,
                                       &mesh->topology.boundbox);
  mesh_shader_build_mvp(mesh, MeshShader_Fixed, scene->ubo);
}

/**

     ▗▄▖ ▗▄▄▄ ▗▄▄▄       ▞   ▗▄▄▖ ▗▄▄▄▖▗▖  ▗▖ ▗▄▖ ▗▖  ▗▖▗▄▄▄▖
    ▐▌ ▐▌▐▌  █▐▌  █     ▞    ▐▌ ▐▌▐▌   ▐▛▚▞▜▌▐▌ ▐▌▐▌  ▐▌▐▌
    ▐▛▀▜▌▐▌  █▐▌  █    ▞     ▐▛▀▚▖▐▛▀▀▘▐▌  ▐▌▐▌ ▐▌▐▌  ▐▌▐▛▀▀▘
    ▐▌ ▐▌▐▙▄▄▀▐▙▄▄▀   ▞      ▐▌ ▐▌▐▙▄▄▖▐▌  ▐▌▝▚▄▞▘ ▝▚▞▘ ▐▙▄▄▖


 */

static inline void scene_system_add_sem(Scene *, Renderer *,
                                        SceneEditorMeshList *);

static inline void scene_system_enable_mesh_in_pipelines(Scene *, Renderer *,
                                                         const MeshRefList *,
                                                         Mesh *);

static inline void scene_system_add_mesh_core(Scene *, Renderer *, Mesh *,
                                              const RendererPipeline,
                                              const char *, const SceneAddFlag);

void scene_system_add_mesh_core(Scene *scene, Renderer *renderer, Mesh *mesh,
                                const RendererPipeline pipeline,
                                const char *layer, const SceneAddFlag flag) {

  {
    // add to scene layers ('Default' layer if NULL)
    if (layer == NULL)
      layer = SCENE_LAYER_DEFAULT;
    scene_layer_set_insert_mesh(&scene->layers, layer, mesh);
  }

  MeshRefList *pipeline_mesh_list = renderer_pipeline(renderer, pipeline);

  // actually show the mesh
  if ((flag & SceneAddFlag_Hide) == 0) {
    mesh_ref_list_insert(pipeline_mesh_list, mesh);
    scene_system_enable_mesh_in_pipelines(scene, renderer, pipeline_mesh_list,
                                          mesh);
  }

  // Update Shadow maps if added to Dynamic_Lit pipeline
  SceneSelectionType selection_pipeline = SceneSelectionType_Mesh;

  if ((pipeline & RendererPipeline_Dynamic_LitShadow) &&
      renderer->draw_mode == RendererDrawMode_Texture) {
    renderer_draw_shadow_map_all(
        &(ShadowMapDrawAllDescriptor){
            .mesh_list = pipeline_mesh_list,
            .lights = &scene->lights,
            .profiler = &renderer->profiler,
        },
        SCENE_DEBUG_UNDEFINED);

    selection_pipeline = SceneSelectionType_MeshShadow;
  }

  // EDITORONLY (add mesh to selection)
  if ((flag & SceneAddFlag_Unselectable) == 0)
    scene_selection_register_mesh(&scene->selection, mesh, mesh->id,
                                  selection_pipeline);

  // EDITORONLY GLUEME
  // if ((flag & SceneAddFlag_TreeHide) == 0 && mesh->parent == NULL)
  //  scene_editor_ui_tree_insert(&scene->editor.ui.tree, mesh->id);
}

/**
  Update passes draw list (sync with their respective scene pipeline)
 */
void scene_system_enable_mesh_in_pipelines(Scene *scene, Renderer *renderer,
                                           const MeshRefList *pipeline,
                                           Mesh *mesh) {

  for (RendererDrawMode i = 0; i < RENDERER_DRAW_MODE_COUNT; i++)
    render_pass_list_enable_mesh(&renderer->mesh_pass[i], mesh);

  render_pass_enable_mesh(&scene->probes.reflection_probe.pass, mesh);
  render_pass_enable_mesh(&scene->probes.reflection_plane.pass, mesh);
  render_pass_enable_mesh(&scene->lights.point.shadow.pass, mesh);
  render_pass_enable_mesh(&scene->lights.spot.shadow.pass, mesh);

  scene_stat_update_draw_call_count(scene, 0);
  scene_stat_update_vertex_count(scene, 0);
}

// === Add Light ===
SceneEditorMeshList *scene_system_add_point_light(Scene *scene,
                                                  Renderer *renderer,
                                                  PointLightDescriptor *desc,
                                                  const LightCreateFlag flag,
                                                  PointLight **dest) {
  PointLight *light;
  SceneEditorMeshList *sem_list =
      scene_add_point_light(scene, desc, flag, &light);

  if (dest)
    *dest = light;

  // recompute shadow map if render mode
  if ((flag & LightCreateFlag_Shadow) &&
      renderer_draw_mode(renderer) == RendererDrawMode_Texture) {

    PointLightListShadow *shadow_list = &scene->lights.point.shadow;
    renderer_draw_shadow_map_point_light(
        &(ShadowMapDrawPointLightDescriptor){
            .light = light,
            .pass = &scene->lights.point.shadow.pass,
            .texture_layer = shadow_list->length,
            .command_encoder = NULL,
            .profiler = &renderer->profiler,
        },
        SCENE_DEBUG_UNDEFINED);
  }

  scene_system_add_sem(scene, renderer, sem_list);

  return sem_list;
}

SceneEditorMeshList *scene_system_add_spot_light(Scene *scene,
                                                 Renderer *renderer,
                                                 SpotLightDescriptor *desc,
                                                 const LightCreateFlag flag,
                                                 SpotLight **dest) {
  SpotLight *light;
  SceneEditorMeshList *sem_list =
      scene_add_spot_light(scene, desc, flag, &light);

  if (dest)
    light = *dest;

  if ((flag & LightCreateFlag_Shadow) &&
      renderer_draw_mode(renderer) == RendererDrawMode_Texture) {

    SpotLightListShadow *shadow_list = &scene->lights.spot.shadow;
    renderer_draw_shadow_map_spot_light(
        &(ShadowMapDrawSpotLightDescriptor){
            .light = light,
            .pass = &shadow_list->pass,
            .texture_layer = shadow_list->length,
            .command_encoder = NULL,
            .profiler = &renderer->profiler,
        },
        SCENE_DEBUG_UNDEFINED);
  }

  scene_system_add_sem(scene, renderer, sem_list);

  return sem_list;
}

SceneEditorMeshList *scene_system_add_sun_light(Scene *scene,
                                                Renderer *renderer,
                                                SunLightDescriptor *desc,
                                                const LightCreateFlag flag,
                                                SunLight **dest) {
  SunLight *light;
  SceneEditorMeshList *sem_list =
      scene_add_sun_light(scene, desc, flag, &light);

  if (dest)
    *dest = light;

  // recompute shadow map if render mode
  if ((flag & LightCreateFlag_Shadow) &&
      renderer_draw_mode(renderer) == RendererDrawMode_Texture) {

    SunLightListShadow *shadow_list = &scene->lights.sun.shadow;
    const size_t target_index =
        light_list_sun_layer_index(&scene->lights, shadow_list->length);

    renderer_draw_shadow_map_sun_light(
        &(ShadowMapDrawSunLightDescriptor){
            .light = light,
            .pass = &scene->lights.spot.shadow.pass,
            .texture_layer = target_index,
            .command_encoder = NULL,
            .profiler = &renderer->profiler,
        },
        SCENE_DEBUG_UNDEFINED);
  }

  scene_system_add_sem(scene, renderer, sem_list);

  return sem_list;
}

SceneEditorMeshList *
scene_system_add_ambient_light(Scene *scene, Renderer *renderer,
                               AmbientLightDescriptor *desc,
                               AmbientLight **dest) {

  SceneEditorMeshList *sem_list = scene_add_ambient_light(scene, desc, dest);
  scene_system_add_sem(scene, renderer, sem_list);

  return sem_list;
}

// === Add Probe ===
SceneEditorMeshList *
scene_system_add_probe_reflection_grid(Scene *scene, Renderer *renderer,
                                       ProbeReflectionGridDescriptor *desc,
                                       ProbeReflectionGrid **dest) {

  SceneEditorMeshList *sem_list =
      scene_add_probe_reflection_grid(scene, desc, dest);
  scene_system_add_sem(scene, renderer, sem_list);

  return sem_list;
}

SceneEditorMeshList *
scene_system_add_probe_reflection_plane(Scene *scene, Renderer *renderer,
                                        ProbeReflectionPlaneDescriptor *desc,
                                        ProbeReflectionPlane **dest) {

  // add draw callback if first probe
  if (scene->probes.reflection_plane.length == 0)
    renderer_add_draw_callback(renderer, renderer_draw_plane_reflection,
                               (void *)scene, RendererDrawMode_Texture);

  SceneEditorMeshList *sem_list =
      scene_add_probe_reflection_plane(scene, desc, dest);
  scene_system_add_sem(scene, renderer, sem_list);

  return sem_list;
}

// === Add Camera ===
SceneEditorMeshList *scene_system_add_camera(Scene *scene, Renderer *renderer,
                                             const CameraCreateDescriptor *desc,
                                             Camera **dest) {

  SceneEditorMeshList *sem_list = scene_add_camera(scene, desc, dest);
  scene_system_add_sem(scene, renderer, sem_list);

  return sem_list;
}

/**
   Compared to casual meshes, Scene Editor Objects (lights/ camera) need to
   follow a more specific path when it comes to be added to the scene and
   especially how they are handled for selection.

   SEM are segmented into:
   1. Meshes list: a visual helper/ representaiton of the entity
   2. Target: the actual data

   As a result in order to add them to the scene we need to add their meshes
   list in the scene pool, but need to add them in a different "branch" of the
   selection system (SceneSelectionType_SEM).

   We use the below function to do such operation.
 */
void scene_system_add_sem(Scene *scene, Renderer *renderer,
                          SceneEditorMeshList *list) {

  MeshRefList *pipeline_mesh_list =
      renderer_pipeline(renderer, RendererPipeline_Fixed);

  for (size_t i = 0; i < list->length; i++) {
    SceneEditorMesh *sem = &list->entries[i];
    Mesh *mesh = sem->mesh;

    {
      scene_add_mesh(scene, mesh, NULL, SceneAddFlag_None);
      scene_system_build_mesh(scene, mesh, RendererPipeline_Fixed);
    }

    {
      // insert to scene pipeline and show it
      mesh_ref_list_insert(pipeline_mesh_list, mesh);
      scene_system_enable_mesh_in_pipelines(scene, renderer, pipeline_mesh_list,
                                            mesh);
    }

    {
      // add to scene selection (SEM pipeline) with target
      scene_selection_register_mesh(&scene->selection, mesh, list->id,
                                    SceneSelectionType_SEM);
    }
  }

  // EDITORONLY
  // DELETE ME scene_editor_ui_tree_insert(&scene->editor.ui.tree, list->id);
}

// === Add Mesh ===

/*
   Automatically map renderer pipeline based on mesh pso pointer
 */
SceneStatus scene_system_add_mesh(Scene *scene, Renderer *renderer, Mesh *mesh,
                                  const char *layer, const SceneAddFlag flag) {

  SceneStatus add_result = scene_add_mesh(scene, mesh, layer, flag);

  const RenderPipeline *mesh_pso =
      mesh_shader(mesh, MeshShader_Texture)->pipeline;

  RendererPipeline pipeline = renderer_get_pso_pipeline(mesh_pso);

  if (pipeline == RendererPipeline_Undefined)
    return SceneStatus_UnvalidPipeline;

  // build mesh depending on pipeline and scene render mode
  scene_system_build_mesh(scene, mesh, pipeline);
  scene_system_add_mesh_core(scene, renderer, mesh, pipeline, layer, flag);

  return add_result;
}

void scene_system_add_mesh_ref_list(Scene *scene, Renderer *renderer,
                                    MeshRefList *list, const char *layer,
                                    const SceneAddFlag flag) {

  scene_add_mesh_ref_list(scene, list, layer, flag);
}

/**
   Function mostly used for Scene Editor Objects like Gizmo, Lights and Camera.
   Casual Meshes go through the scene_add_mesh(...) function.

   The key difference is that the scen_add_mesh will dispatch/ define the mesh
   to the scene pipeline automatically based on the pipeline pointer (if
   pipeline == pbr, then goes to lit shadow scene list). However in this fixed
   method, we provide the scene pipeline so it will stay the same no matter the
   render draw mode.
 */
void scene_system_add_mesh_pipeline(Scene *scene, Renderer *renderer,
                                    Mesh *mesh, const RendererPipeline pipeline,
                                    const char *layer,
                                    const SceneAddFlag flag) {
  
  SceneStatus add_result = scene_add_mesh(scene, mesh, layer, flag);
  scene_system_build_mesh(scene, mesh, pipeline);
  scene_system_add_mesh_core(scene, renderer, mesh, pipeline, layer, flag);
  
}

/**
   DELETEME ?
   Add a list of mesh pointers (presumably from the scene mesh pool) to a
   pipeline. Meaning each meshes are going to be build depending on the pipeline
   and the current render mode.
 */
void scene_system_add_mesh_pipeline_ref_list(Scene *scene, Renderer *renderer,
                                             MeshRefList *list,
                                             const RendererPipeline pipeline,
                                             const char *layer,
                                             const SceneAddFlag flag) {

  for (size_t i = 0; i < list->length; i++)
    scene_system_add_mesh_pipeline(scene, renderer, list->entries[i], pipeline,
                                   layer, flag);
}

// === Remove Mesh ===
void scene_system_remove_mesh(Scene *scene, Renderer *renderer, Mesh *mesh) {

  scene_remove_mesh(scene, mesh);
}

void scene_system_remove_mesh_ref_list(Scene *scene, Renderer *renderer,
                                       MeshRefList *list) {

  scene_remove_mesh_ref_list(scene, list);
}

void scene_system_create_grid(Scene *scene, Renderer *renderer) {

  Mesh *grid = rem_new_mesh();
  sem_grid_create(grid, &(GridUniform){
                            .size = 100.0f,
                            .cell_size = 100.0f,
                            .thickness = 44.0f,
                            .color = {0.5f, 0.5f, 0.5f, 1.0f},
                        });

  scene_system_add_mesh_pipeline(scene, renderer, grid, RendererPipeline_Fixed,
                                 NULL, SceneAddFlag_Unselectable);

  scene->grid = grid;
}
