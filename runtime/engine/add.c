#include "add.h"
#include "backend/logger.h"
#include "backend/renderer/reflection/draw.h"
#include "backend/renderer/shadow_map/draw.h"
#include "backend/resource_manager.h"
#include "runtime/engine/build.h"
#include "runtime/engine/core.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"
#include "runtime/scene/stat.h"

static inline void engine_scene_add_sem(Engine *, SceneEditorMeshList *);

static inline void engine_enable_mesh_in_pipelines(Engine *,
                                                   const MeshRefList *, Mesh *);

static inline void engine_scene_add_mesh_core(Engine *, Mesh *,
                                              const RendererPipeline,
                                              const char *,
                                              const EngineAddFlag);

Scene *engine_add_scene(Engine *engine, const SceneCreateDescriptor *desc) {

  if (engine->scenes.length >= ENGINE_SCENE_CAPACITY) {
    logger_add(LoggerFlag_Error, "Engine reached max scene capacity (%lu)",
               ENGINE_SCENE_CAPACITY);
    return NULL;
  }

  Scene *scene = rem_new_scene();
  scene_create(scene, desc);

  if (!scene) {
    logger_add(LoggerFlag_Error,
               "Resource manager unable to provide a new scene.",
               ENGINE_SCENE_CAPACITY);
    return NULL;
  }

  engine->scenes.entries[engine->scenes.length++] = scene;

  return scene;
}

void engine_scene_add_mesh_core(Engine *engine, Mesh *mesh,
                                const RendererPipeline pipeline,
                                const char *layer, const EngineAddFlag flag) {

  Scene *scene = engine_get_active_scene(engine);
  Renderer *renderer = engine_get_renderer(engine);

  {
    // add to scene layers ('Default' layer if NULL)
    if (layer == NULL)
      layer = SCENE_LAYER_DEFAULT;
    scene_layer_set_insert_mesh(&scene->layers, layer, mesh);
  }

  MeshRefList *pipeline_mesh_list = renderer_pipeline(renderer, pipeline);

  // actually show the mesh
  if ((flag & EngineAddFlag_Hide) == 0) {
    mesh_ref_list_insert(pipeline_mesh_list, mesh);
    engine_enable_mesh_in_pipelines(engine, pipeline_mesh_list, mesh);
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
  if ((flag & EngineAddFlag_Unselectable) == 0)
    scene_selection_register_mesh(&scene->selection, mesh, mesh->id,
                                  selection_pipeline);

  // EDITORONLY
  if ((flag & EngineAddFlag_TreeHide) == 0 && mesh->parent == NULL)
    gui_tree_insert(&engine->gui->tree, mesh->id);
}

/**
  Update passes draw list (sync with their respective scene pipeline)
 */
void engine_enable_mesh_in_pipelines(Engine *engine,
                                     const MeshRefList *pipeline, Mesh *mesh) {

  Scene *scene = engine_get_active_scene(engine);
  Renderer *renderer = engine_get_renderer(engine);

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
SceneEditorMeshList *engine_scene_add_point_light(Engine *engine,
                                                  PointLightDescriptor *desc,
                                                  const LightCreateFlag flag,
                                                  PointLight **dest) {

  Scene *scene = engine_get_active_scene(engine);
  Renderer *renderer = engine_get_renderer(engine);

  PointLight *light;
  SceneEditorMeshList *sem_list =
      scene_add_point_light(scene, desc, LightCreateFlag_None, &light);

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

  engine_scene_add_sem(engine, sem_list);

  return sem_list;
}

SceneEditorMeshList *engine_scene_add_spot_light(Engine *engine,
                                                 SpotLightDescriptor *desc,
                                                 const LightCreateFlag flag,
                                                 SpotLight **dest) {

  Scene *scene = engine_get_active_scene(engine);
  Renderer *renderer = engine_get_renderer(engine);

  SpotLight *light;
  SceneEditorMeshList *sem_list =
      scene_add_spot_light(scene, desc, LightCreateFlag_None, &light);

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

  engine_scene_add_sem(engine, sem_list);

  return sem_list;
}

SceneEditorMeshList *engine_scene_add_sun_light(Engine *engine,
                                                SunLightDescriptor *desc,
                                                const LightCreateFlag flag,
                                                SunLight **dest) {

  Scene *scene = engine_get_active_scene(engine);
  Renderer *renderer = engine_get_renderer(engine);

  SunLight *light;
  SceneEditorMeshList *sem_list =
      scene_add_sun_light(scene, desc, LightCreateFlag_None, &light);

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

  engine_scene_add_sem(engine, sem_list);

  return sem_list;
}

SceneEditorMeshList *
engine_scene_add_ambient_light(Engine *engine, AmbientLightDescriptor *desc,
                               AmbientLight **dest) {

  Scene *scene = engine_get_active_scene(engine);
  SceneEditorMeshList *sem_list = scene_add_ambient_light(scene, desc, dest);
  engine_scene_add_sem(engine, sem_list);

  return sem_list;
}

// === Add Probe ===
SceneEditorMeshList *
engine_scene_add_probe_reflection_grid(Engine *engine,
                                       ProbeReflectionGridDescriptor *desc,
                                       ProbeReflectionGrid **dest) {

  Scene *scene = engine_get_active_scene(engine);

  SceneEditorMeshList *sem_list =
      scene_add_probe_reflection_grid(scene, desc, dest);
  engine_scene_add_sem(engine, sem_list);

  return sem_list;
}

SceneEditorMeshList *
engine_scene_add_probe_reflection_plane(Engine *engine,
                                        ProbeReflectionPlaneDescriptor *desc,
                                        ProbeReflectionPlane **dest) {

  Scene *scene = engine_get_active_scene(engine);
  Renderer *renderer = engine_get_renderer(engine);

  // add draw callback if first probe
  if (scene->probes.reflection_plane.length == 0)
    renderer_add_draw_callback(renderer, renderer_draw_plane_reflection,
                               (void *)scene, RendererDrawMode_Texture);

  SceneEditorMeshList *sem_list =
      scene_add_probe_reflection_plane(scene, desc, dest);
  engine_scene_add_sem(engine, sem_list);

  return sem_list;
}

// === Add Camera ===
SceneEditorMeshList *engine_scene_add_camera(Engine *engine,
                                             const CameraCreateDescriptor *desc,
                                             Camera **dest) {

  Scene *scene = engine_get_active_scene(engine);
  SceneEditorMeshList *sem_list = scene_add_camera(scene, desc, dest);
  engine_scene_add_sem(engine, sem_list);

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
void engine_scene_add_sem(Engine *engine, SceneEditorMeshList *list) {

  Scene *scene = engine_get_active_scene(engine);
  Renderer *renderer = engine_get_renderer(engine);

  MeshRefList *pipeline_mesh_list =
      renderer_pipeline(renderer, RendererPipeline_Fixed);

  for (size_t i = 0; i < list->length; i++) {
    SceneEditorMesh *sem = &list->entries[i];
    Mesh *mesh = sem->mesh;

    {
      scene_add_mesh(scene, mesh, NULL);
      engine_build_mesh(engine, mesh, RendererPipeline_Fixed);
    }

    {
      // insert to scene pipeline and show it
      mesh_ref_list_insert(pipeline_mesh_list, mesh);
      engine_enable_mesh_in_pipelines(engine, pipeline_mesh_list, mesh);
    }

    {
      // add to scene selection (SEM pipeline) with target
      scene_selection_register_mesh(&scene->selection, mesh, list->id,
                                    SceneSelectionType_SEM);
    }
  }

  // EDITORONLY
  gui_tree_insert(&engine->gui->tree, list->id);
}

// === Add Mesh ===

/*
   Automatically map renderer pipeline based on mesh pso pointer
 */
EngineStatus engine_scene_add_mesh(Engine *engine, Mesh *mesh,
                                   const char *layer,
                                   const EngineAddFlag flag) {

  Scene *scene = engine_get_active_scene(engine);
  SceneStatus add_result = scene_add_mesh(scene, mesh, layer);

  const RenderPipeline *mesh_pso =
      mesh_shader(mesh, MeshShader_Texture)->pipeline;

  RendererPipeline pipeline = renderer_get_pso_pipeline(mesh_pso);

  if (pipeline == RendererPipeline_Undefined)
    return EngineStatus_InvalidPipeline;

  // build mesh depending on pipeline and scene render mode
  engine_build_mesh(engine, mesh, pipeline);

  engine_scene_add_mesh_core(engine, mesh, pipeline, layer, flag);

  return EngineStatus_Success;
}

// DELETEME ?
EngineStatus engine_scene_add_mesh_ref_list(Engine *engine, MeshRefList *list,
                                            const char *layer,
                                            const EngineAddFlag flag) {

  Scene *scene = engine_get_active_scene(engine);
  scene_add_mesh_ref_list(scene, list, layer);

  return EngineStatus_Success;
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
EngineStatus engine_scene_add_mesh_pipeline(Engine *engine, Mesh *mesh,
                                            const RendererPipeline pipeline,
                                            const char *layer,
                                            const EngineAddFlag flag) {

  Scene *scene = engine_get_active_scene(engine);

  SceneStatus add_result = scene_add_mesh(scene, mesh, layer);
  engine_build_mesh(engine, mesh, pipeline);
  engine_scene_add_mesh_core(engine, mesh, pipeline, layer, flag);

  return EngineStatus_Success;
}

/**
   DELETEME ?
   Add a list of mesh pointers (presumably from the scene mesh pool) to a
   pipeline. Meaning each meshes are going to be build depending on the pipeline
   and the current render mode.
 */
EngineStatus engine_scene_add_mesh_pipeline_ref_list(
    Engine *engine, MeshRefList *list, const RendererPipeline pipeline,
    const char *layer, const EngineAddFlag flag) {

  for (size_t i = 0; i < list->length; i++)
    engine_scene_add_mesh_pipeline(engine, list->entries[i], pipeline, layer,
                                   flag);

  return EngineStatus_Success;
}

// === Remove Mesh ===
EngineStatus engine_scene_remove_mesh(Engine *engine, Mesh *mesh) {

  Scene *scene = engine_get_active_scene(engine);
  scene_remove_mesh(scene, mesh);

  return EngineStatus_Success;
}

EngineStatus engine_scene_remove_mesh_ref_list(Engine *engine,
                                               MeshRefList *list) {

  Scene *scene = engine_get_active_scene(engine);
  scene_remove_mesh_ref_list(scene, list);

  return EngineStatus_Success;
}
