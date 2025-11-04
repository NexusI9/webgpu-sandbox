#include "add.h"

#include <stdint.h>
#include <stdio.h>

#include "./editor/editor.h"
#include "backend/logger.h"
#include "backend/registry.h"
#include "backend/std_pipeline/core.h"
#include "backend/ubo.h"
#include "build.h"
#include "core.h"
#include "debug/core.h"
#include "editor/mesh/camera/camera.h"
#include "editor/mesh/light/ambient.h"
#include "editor/mesh/light/point.h"
#include "editor/mesh/light/spot.h"
#include "editor/mesh/light/sun.h"
#include "editor/mesh/list/list.h"
#include "editor/mesh/probe/reflection_grid.h"
#include "editor/mesh/probe/reflection_plane.h"
#include "editor/selection/core.h"
#include "layer.h"
#include "renderer/core.h"
#include "runtime/camera/core.h"
#include "runtime/camera/list.h"
#include "runtime/light/core.h"
#include "runtime/light/list.h"
#include "runtime/light/shadow_map/draw.h"
#include "runtime/light/uniform.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/list.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/mesh/uniform.h"
#include "runtime/pipeline/render.h"
#include "runtime/probe/reflection/core.h"
#include "runtime/probe/reflection/draw.h"
#include "runtime/probe/reflection/grid.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/probe/reflection/probe.h"
#include "runtime/probe/uniform.h"
#include "backend/renderer/render_pass/visibility.h"
#include "runtime/scene/stat.h"
#include "utils/projection.h"

static inline void scene_add_sem(Scene *, SceneEditorMeshList *);
static inline void
scene_render_pass_draw_list_enable_mesh(Scene *, const MeshRefList *, Mesh *);
static inline ScenePipeline scene_map_pipeline(const RenderPipeline *);

/**
    ▗▄▄▖ ▗▄▄▖▗▄▄▄▖▗▖  ▗▖▗▄▄▄▖    ▗▄▄▄▖▗▄▄▄ ▗▄▄▄▖▗▄▄▄▖▗▄▖ ▗▄▄▖
   ▐▌   ▐▌   ▐▌   ▐▛▚▖▐▌▐▌       ▐▌   ▐▌  █  █    █ ▐▌ ▐▌▐▌ ▐▌
    ▝▀▚▖▐▌   ▐▛▀▀▘▐▌ ▝▜▌▐▛▀▀▘    ▐▛▀▀▘▐▌  █  █    █ ▐▌ ▐▌▐▛▀▚▖
   ▗▄▄▞▘▝▚▄▄▖▐▙▄▄▖▐▌  ▐▌▐▙▄▄▖    ▐▙▄▄▖▐▙▄▄▀▗▄█▄▖  █ ▝▚▄▞▘▐▌ ▐▌

              ▗▄▖ ▗▄▄▖    ▗▖▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖▗▄▄▖
             ▐▌ ▐▌▐▌ ▐▌   ▐▌▐▌   ▐▌     █ ▐▌
             ▐▌ ▐▌▐▛▀▚▖   ▐▌▐▛▀▀▘▐▌     █  ▝▀▚▖
             ▝▚▄▞▘▐▙▄▞▘▗▄▄▞▘▐▙▄▄▖▝▚▄▄▖  █ ▗▄▄▞▘

 */
SceneEditorMeshList *scene_add_point_light(Scene *scene,
                                           PointLightDescriptor *desc,
                                           const LightCreateFlag flag,
                                           PointLight **dest) {

  PointLightListBase *base_list = &scene->lights.point.base;

  // create sun light
  PointLight *light = light_list_new_point_light(base_list);

  if (light == NULL) {
    logger_add(LoggerFlag_Error, "Scene spot light capacity reached maximum.");
    return NULL;
  }

  point_light_create(light, desc);

  if (dest)
    *dest = light;

  light->ubo_uniform = light_list_uniform_new_entry(
      scene->lights.ubo_slot.uniform, LightType_Point);

  point_light_uniform_update(light);

  // create mesh/gizmo
  SceneEditorMeshList *sem =
      sem_list_array_new_entry(scene_editor_mesh_list(&scene->editor),
                               RegEntryType_SceneEditorMeshList_PointLight);

  SEMCreateDescriptor sem_desc = {
      .camera = scene->active_camera,
      .viewport = &scene->viewport,
      .scene = scene,
      .target_list_index = 0,
  };

  if (flag & LightCreateFlag_Shadow) {

    for (uint8_t i = 0; i < PROJECTION_VIEW_COUNT; i++)
      light->ubo_projection[i] =
          ubo_new_entry(scene->ubo, UBOType_ViewProjection);

    point_light_projection_update(light);

    for (uint8_t i = 0; i < PROJECTION_VIEW_COUNT; i++)
      ubo_upload_entry(scene->ubo, UBOType_ViewProjection,
                       &light->ubo_projection[i]);

    PointLightListShadow *shadow_list = &scene->lights.point.shadow;

    sem_desc.target_list_index = shadow_list->length;
    sem_point_light_shadow_create(sem, light, &sem_desc);

    light_list_point_shadow_insert(shadow_list, light);

    // recompute shadow map if render mode
    if (renderer_draw_mode(&scene->renderer) ==
        RendererDrawMode_Texture)
      shadow_map_draw_point_light(
          &(ShadowMapDrawPointLightDescriptor){
              .light = light,
              .pass = &scene->lights.point.shadow.pass,
              .texture_layer = shadow_list->length,
              .command_encoder = NULL,
              .profiler = &scene->renderer.profiler,
          },
          SCENE_DEBUG_UNDEFINED);

  } else {
    sem_point_light_create(sem, light, &sem_desc);
  }

  // transfert gizmo mesh pointers to scene pipeline so they get rendered
  scene_add_sem(scene, sem);

  light_list_uniform_update(&scene->lights);
  ubo_upload_entry(scene->ubo, UBOType_LightList,
                   &scene->lights.ubo_slot);

  return sem;
}

SceneEditorMeshList *scene_add_spot_light(Scene *scene,
                                          SpotLightDescriptor *desc,
                                          const LightCreateFlag flag,
                                          SpotLight **dest) {

  SpotLightListBase *base_list = &scene->lights.spot.base;

  // create sun light
  SpotLight *light = light_list_new_spot_light(base_list);

  if (light == NULL) {
    logger_add(LoggerFlag_Error, "Scene spot light capacity reached maximum.");
    return NULL;
  }

  spot_light_create(light, desc);

  if (dest)
    *dest = light;

  light->ubo_uniform = light_list_uniform_new_entry(
      scene->lights.ubo_slot.uniform, LightType_Spot);
  spot_light_uniform_update(light);

  // create mesh/gizmo
  SceneEditorMeshList *sem =
      sem_list_array_new_entry(scene_editor_mesh_list(&scene->editor),
                               RegEntryType_SceneEditorMeshList_SpotLight);

  SEMCreateDescriptor sem_desc = {
      .camera = scene->active_camera,
      .viewport = &scene->viewport,
      .scene = scene,
      .target_list_index = 0,
  };

  if (flag & LightCreateFlag_Shadow) {

    light->ubo_projection =
        ubo_new_entry(scene->ubo, UBOType_ViewProjection);

    spot_light_projection_update(light);
    ubo_upload_entry(scene->ubo, UBOType_ViewProjection,
                     &light->ubo_projection);

    SpotLightListShadow *shadow_list = &scene->lights.spot.shadow;

    sem_desc.target_list_index = shadow_list->length;
    sem_spot_light_shadow_create(sem, light, &sem_desc);

    light_list_spot_shadow_insert(shadow_list, light);

    // recompute shadow map if render mode
    if (renderer_draw_mode(&scene->renderer) ==
        RendererDrawMode_Texture)
      shadow_map_draw_spot_light(
          &(ShadowMapDrawSpotLightDescriptor){
              .light = light,
              .pass = &shadow_list->pass,
              .texture_layer = shadow_list->length,
              .command_encoder = NULL,
              .profiler = &scene->renderer.profiler,
          },
          SCENE_DEBUG_UNDEFINED);

  } else {
    sem_spot_light_create(sem, light, &sem_desc);
  }

  // transfert gizmo mesh pointers to scene pipeline so they get rendered
  scene_add_sem(scene, sem);

  // update UBO
  light_list_uniform_update(&scene->lights);
  ubo_upload_entry(scene->ubo, UBOType_LightList,
                   &scene->lights.ubo_slot);

  return sem;
}

SceneEditorMeshList *scene_add_ambient_light(Scene *scene,
                                             AmbientLightDescriptor *desc,
                                             AmbientLight **dest) {

  AmbientLightList *list = &scene->lights.ambient;

  // create sun light
  AmbientLight *light = light_list_new_ambient_light(list);

  if (light == NULL) {
    logger_add(LoggerFlag_Error, "Scene sun light capacity reached maximum.");
    return NULL;
  }

  ambient_light_create(light, desc);

  if (dest)
    *dest = light;

  light->ubo_uniform = light_list_uniform_new_entry(
      scene->lights.ubo_slot.uniform, LightType_Ambient);

  ambient_light_uniform_update(light);

  // create mesh/gizmo
  SceneEditorMeshList *sem =
      sem_list_array_new_entry(scene_editor_mesh_list(&scene->editor),
                               RegEntryType_SceneEditorMeshList_AmbientLight);

  sem_ambient_light_create(sem, light,
                           &(SEMCreateDescriptor){
                               .camera = scene->active_camera,
                               .viewport = &scene->viewport,
                               .scene = scene,
                               .target_list_index = list->length - 1,
                           });

  // transfert gizmo mesh pointers to scene pipeline so they get rendered
  scene_add_sem(scene, sem);

  light_list_uniform_update(&scene->lights);
  ubo_upload_entry(scene->ubo, UBOType_LightList,
                   &scene->lights.ubo_slot);

  return sem;
}

SceneEditorMeshList *scene_add_sun_light(Scene *scene, SunLightDescriptor *desc,
                                         const LightCreateFlag flag,
                                         SunLight **dest) {

  SunLightListBase *base_list = &scene->lights.sun.base;

  // create sun light
  SunLight *light = light_list_new_sun_light(base_list);

  if (light == NULL) {
    logger_add(LoggerFlag_Error, "Scene sun light capacity reached maximum.");
    return NULL;
  }

  sun_light_create(light, desc);

  if (dest)
    *dest = light;

  light->ubo_uniform = light_list_uniform_new_entry(
      scene->lights.ubo_slot.uniform, LightType_Sun);

  sun_light_uniform_update(light);

  // create mesh/gizmo
  SceneEditorMeshList *sem =
      sem_list_array_new_entry(scene_editor_mesh_list(&scene->editor),
                               RegEntryType_SceneEditorMeshList_SunLight);

  SEMCreateDescriptor sem_desc = {
      .camera = scene->active_camera,
      .viewport = &scene->viewport,
      .scene = scene,
      .target_list_index = 0,
  };

  if (flag & LightCreateFlag_Shadow) {

    light->ubo_projection =
        ubo_new_entry(scene->ubo, UBOType_ViewProjection);

    sun_light_projection_update(light);
    ubo_upload_entry(scene->ubo, UBOType_ViewProjection,
                     &light->ubo_projection);

    SunLightListShadow *shadow_list = &scene->lights.sun.shadow;

    sem_desc.target_list_index = shadow_list->length;

    sem_sun_light_shadow_create(sem, light, &sem_desc);

    light_list_sun_shadow_insert(shadow_list, light);

    // recompute shadow map if render mode
    if (renderer_draw_mode(&scene->renderer) ==
        RendererDrawMode_Texture)

      shadow_map_draw_sun_light(
          &(ShadowMapDrawSunLightDescriptor){
              .light = light,
              .pass = &scene->lights.spot.shadow.pass,
              .texture_layer = light_list_sun_layer_index(
                  &scene->lights, sem_desc.target_list_index),
              .command_encoder = NULL,
              .profiler = &scene->renderer.profiler,
          },
          SCENE_DEBUG_UNDEFINED);

  } else {
    sem_sun_light_create(sem, light, &sem_desc);
  }

  scene_add_sem(scene, sem);

  light_list_uniform_update(&scene->lights);
  ubo_upload_entry(scene->ubo, UBOType_LightList,
                   &scene->lights.ubo_slot);

  return sem;
}

/**
   "Create" a new uninitialized camera in the scene camera list and return the
   newly created item's pointer.

        ScenePool
       .---------.
       |   ...   |
       |---------|              SEM<T>
       |  cam N  | ---.      .-----------.       Scene Render Layer
       '---------'    '--->  |   target  |           .-----------.
                      .--->  |  meshes*  | --------> |  mesh 1*  |
                      |      '-----------'           |-----------|
    Scene Mesh Pool   |                              |  mesh 2*  |
       .----------.   |                              |-----------|
       |   ...    |   |                              |    ...    |
       |----------|   |                              '-----------'
       |  mesh N  | --'
       '----------'

 */
SceneEditorMeshList *scene_add_camera(Scene *scene,
                                      const CameraCreateDescriptor *desc,
                                      Camera **dest) {

  // init scene camera
  Camera *new_cam = camera_list_new_camera(&scene->cameras);
  camera_create(new_cam, desc);

  if (dest)
    *dest = new_cam;

  // create gizmo
  SceneEditorMeshList *sem =
      sem_list_array_new_entry(scene_editor_mesh_list(&scene->editor),
                               RegEntryType_SceneEditorMeshList_Camera);

  sem_camera_create(sem, new_cam,
                    &(SEMCreateDescriptor){
                        .camera = scene->active_camera,
                        .viewport = &scene->viewport,
                        .scene = scene,
                        .target_list_index = scene->cameras.length - 1,
                    });

  // transfert gizmo mesh pointers to scene pipeline so they get rendered
  scene_add_sem(scene, sem);

  return sem;
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
void scene_add_sem(Scene *scene, SceneEditorMeshList *list) {

  MeshRefList *pipeline_mesh_list = scene_pipeline(scene, ScenePipeline_Fixed);

  for (size_t i = 0; i < list->length; i++) {
    SceneEditorMesh *sem = &list->entries[i];
    Mesh *mesh = sem->mesh;

    {
      // build mesh depending on pipeline and scene render mode
      mesh->ubo_slot = ubo_new_entry(scene->ubo, UBOType_Mesh);
      mesh_uniform_update(mesh);
      ubo_upload_entry(scene->ubo, UBOType_Mesh, &mesh->ubo_slot);
      scene_build_mesh(scene, mesh, ScenePipeline_Fixed);
    }

    {
      // insert to scene pipeline and show it
      mesh_ref_list_insert(pipeline_mesh_list, mesh);
      scene_render_pass_draw_list_enable_mesh(scene, pipeline_mesh_list, mesh);
    }

    {
      // add to scene selection (SEM pipeline) with target
      scene_selection_subscribe_mesh(&scene->editor.selection, mesh, list->id,
                                     SceneSelectionType_SEM);
    }
  }

  // EDITORONLY
  // DELETE ME scene_editor_ui_tree_insert(&scene->editor.ui.tree, list->id);
}

SceneEditorMeshList *
scene_add_probe_reflection_grid(Scene *scene,
                                ProbeReflectionGridDescriptor *desc,
                                ProbeReflectionGrid **dest) {

  ProbeReflectionGrid *new_grid =
      probe_reflection_grid_list_new_entry(&scene->probes.reflection_probe);

  probe_reflection_grid_create(new_grid, desc);

  if (dest)
    *dest = new_grid;

  // create scene object
  SceneEditorMeshList *sem_grid = sem_list_array_new_entry(
      scene_editor_mesh_list(&scene->editor),
      RegEntryType_SceneEditorMeshList_ProbeReflectionGrid);

  sem_probe_reflection_grid_create(sem_grid, new_grid,
                                   &(SEMCreateDescriptor){
                                       .camera = scene->active_camera,
                                       .viewport = &scene->viewport,
                                       .scene = scene,
                                       .target_list_index = 0,
                                   });

  // add probes to ubo list
  for (uint16_t i = 0; i < new_grid->probes.length; i++) {
    ProbeReflection *probe = new_grid->probes.entries[i];
    UBOManager *ubo = scene->ubo;

    {
      probe->ubo_uniform = probe_list_uniform_new_entry(
          scene->probes.ubo_slot.uniform, ProbeType_ReflectionProbe);
      probe_reflection_update_uniform(probe);
    }

    {
      // add each views
      for (uint8_t v = 0; v < PROBE_REFLECTION_VIEW_COUNT; v++)
        probe->ubo_camera[v] =
            ubo_new_entry(scene->ubo, UBOType_Camera);
      probe_reflection_update_camera(probe);
    }
  }

  // update UBO for probe count
  probe_list_update_uniform(&scene->probes);
  ubo_upload_entry(scene->ubo, UBOType_ProbeList,
                   &scene->probes.ubo_slot);

  // transfert gizmo mesh pointers to scene pipeline so they get rendered
  scene_add_sem(scene, sem_grid);

  return sem_grid;
}

SceneEditorMeshList *
scene_add_probe_reflection_plane(Scene *scene,
                                 ProbeReflectionPlaneDescriptor *desc,
                                 ProbeReflectionPlane **dest) {

  // && scene->renderer.draw.mode == RendererDrawMode_Texture
  // add draw callback if first probe
  if (scene->probes.reflection_plane.length == 0)
    renderer_add_draw_callback(
        &scene->renderer, probe_reflection_plane_list_draw_callback,
        (void *)scene, RendererDrawMode_Texture);

  ProbeReflectionPlane *probe =
      probe_reflection_plane_list_new_entry(&scene->probes.reflection_plane);

  if (dest)
    *dest = probe;

  probe_reflection_plane_create(probe, desc);

  // create scene object
  SceneEditorMeshList *sem = sem_list_array_new_entry(
      scene_editor_mesh_list(&scene->editor),
      RegEntryType_SceneEditorMeshList_ProbeReflectionPlane);

  probe_reflection_plane_create(probe, desc);

  sem_probe_reflection_plane_create(
      sem, probe,
      &(SEMCreateDescriptor){
          .camera = scene->active_camera,
          .viewport = &scene->viewport,
          .scene = scene,
          .target_list_index = SCENE_EDITOR_MESH_TARGET_UNDEFINED,
      });

  // add probes to ubo list
  UBOManager *ubo = scene->ubo;

  {
    probe->ubo_uniform = probe_list_uniform_new_entry(
        scene->probes.ubo_slot.uniform, ProbeType_ReflectionPlane);
    probe_reflection_plane_update_uniform(probe);
  }

  {
    probe->ubo_camera = ubo_new_entry(scene->ubo, UBOType_Camera);
    probe_reflection_plane_update_camera(probe);
  }

  // update Probe List UBO for probe count
  probe_list_update_uniform(&scene->probes);
  ubo_upload_entry(scene->ubo, UBOType_ProbeList,
                   &scene->probes.ubo_slot);

  // transfert gizmo mesh pointers to scene pipeline so they get rendered
  scene_add_sem(scene, sem);

  return sem;
}

/**
  ▗▖  ▗▖▗▄▄▄▖ ▗▄▄▖▗▖ ▗▖
  ▐▛▚▞▜▌▐▌   ▐▌   ▐▌ ▐▌
  ▐▌  ▐▌▐▛▀▀▘ ▝▀▚▖▐▛▀▜▌
  ▐▌  ▐▌▐▙▄▄▖▗▄▄▞▘▐▌ ▐▌

 */

static inline void scene_add_mesh_core(Scene *, Mesh *, const ScenePipeline,
                                       const char *, const SceneAddFlag);

void scene_add_mesh_core(Scene *scene, Mesh *mesh, const ScenePipeline pipeline,
                         const char *layer, const SceneAddFlag flag) {

  {
    // add to scene layers ('Default' layer if NULL)
    if (layer == NULL)
      layer = SCENE_LAYER_DEFAULT;
    scene_layer_set_insert_mesh(&scene->layers, layer, mesh);
  }

  MeshRefList *pipeline_mesh_list = scene_pipeline(scene, pipeline);

  // actually show the mesh
  if ((flag & SceneAddFlag_Hide) == 0) {
    mesh_ref_list_insert(pipeline_mesh_list, mesh);
    scene_render_pass_draw_list_enable_mesh(scene, pipeline_mesh_list, mesh);
  }

  // Update Shadow maps if added to Dynamic_Lit pipeline
  SceneSelectionType selection_pipeline = SceneSelectionType_Mesh;

  if ((pipeline & ScenePipeline_Dynamic_LitShadow) &&
      scene->renderer.draw.mode == RendererDrawMode_Texture) {
    shadow_map_draw_all(
        &(ShadowMapDrawAllDescriptor){
            .mesh_list = pipeline_mesh_list,
            .lights = &scene->lights,
            .profiler = &scene->renderer.profiler,
        },
        SCENE_DEBUG_UNDEFINED);

    selection_pipeline = SceneSelectionType_MeshShadow;
  }

  // EDITORONLY (add mesh to selection)
  if ((flag & SceneAddFlag_Unselectable) == 0)
    scene_selection_subscribe_mesh(&scene->editor.selection, mesh, mesh->id,
                                   selection_pipeline);

  // EDITORONLY DELETEME
  //if ((flag & SceneAddFlag_TreeHide) == 0 && mesh->parent == NULL)
  //  scene_editor_ui_tree_insert(&scene->editor.ui.tree, mesh->id);
}

/**
  Update passes draw list (sync with their respective scene pipeline)
 */
void scene_render_pass_draw_list_enable_mesh(
    Scene *scene, const MeshRefList *pipeline_mesh_list, Mesh *mesh) {

  for (RendererDrawMode i = 0; i < RENDERER_DRAW_MODE_COUNT; i++)
    render_pass_list_enable_mesh(&scene->renderer.draw.render_pass[i], mesh);

  render_pass_enable_mesh(&scene->probes.reflection_probe.pass, mesh);
  render_pass_enable_mesh(&scene->probes.reflection_plane.pass, mesh);
  render_pass_enable_mesh(&scene->lights.point.shadow.pass, mesh);
  render_pass_enable_mesh(&scene->lights.spot.shadow.pass, mesh);

  scene_stat_update_draw_call_count(scene);
  scene_stat_update_vertex_count(scene);
}

/**
   Add mesh to the dynamic pipeline.
   Depending on the mesh current texture pipeline, it will either dispatch the
   mesh to the unlit or lit pipeline.
   Basically if mesh texture shader has PBR pipeline it goes to the lit, if it
   has Unlit pieline it goes to the unlit.

   To add a mesh to the fixed pipelines (ex: Gizmo, Scene Editor Objects), the
   scene_add_mesh_pipeline dedicated function shall be used.

   The below function is designed for "common usage", meaning on a daily basis,
   one will add dynamic assets to the scene, compared to the fixed elements
   which are only used by the editor itself.
 */
ScenePipeline scene_map_pipeline(const RenderPipeline *render_pipeline) {

  static const ScenePipeline
      scene_pipeline_dispatch[RENDER_PIPELINE_TYPE_COUNT] = {
          // Unlit
          [RenderPipelineType_Billboard] = ScenePipeline_Dynamic_Unlit,
          [RenderPipelineType_Unlit] = ScenePipeline_Dynamic_Unlit,
          [RenderPipelineType_GlassProbeGrid] = ScenePipeline_Dynamic_Unlit,
          [RenderPipelineType_GlassProbePlane] = ScenePipeline_Dynamic_Unlit,

          // Lit
          [RenderPipelineType_Reflection] = ScenePipeline_Dynamic_Lit,

          // Shadow
          [RenderPipelineType_Default] = ScenePipeline_Dynamic_LitShadow,
          [RenderPipelineType_PBR] = ScenePipeline_Dynamic_LitShadow,
          [RenderPipelineType_PBR_DoubleSided] =
              ScenePipeline_Dynamic_LitShadow,

          // Alpha
          [RenderPipelineType_PBR_Alpha] = ScenePipeline_Dynamic_LitAlpha,

          // Fixed
          [RenderPipelineType_Grid] = ScenePipeline_Fixed,
          [RenderPipelineType_Line] = ScenePipeline_Fixed,
          [RenderPipelineType_Screen] = ScenePipeline_Fixed,
          [RenderPipelineType_Shadow] = ScenePipeline_Fixed,
          [RenderPipelineType_Solid] = ScenePipeline_Fixed,
          [RenderPipelineType_Blit] = ScenePipeline_Fixed,

          // Background
          [RenderPipelineType_Skybox] = ScenePipeline_Fixed_Background,

      };

  RenderPipelineType pipeline_type = std_render_pipeline_type(render_pipeline);

  if (pipeline_type == RENDER_PIPELINE_UNDEFINED) {
    logger_add(LoggerFlag_Error,
               "Couldn't find any valid type for mesh pipeline.");
    return ScenePipeline_Undefined;
  }

  // dispatch mesh based on their global pipeline address (lit by default)
  ScenePipeline pipeline = scene_pipeline_dispatch[pipeline_type];

  if (pipeline == ScenePipeline_Undefined)
    logger_add(LoggerFlag_Error,
               "Couldn't find any scene pipeline for render pipeline: %d.");

  return pipeline;
}

SceneStatus scene_add_mesh(Scene *scene, Mesh *mesh, const char *layer,
                           const SceneAddFlag flag) {

  const RenderPipeline *mesh_pipeline =
      mesh_shader(mesh, MeshShader_Texture)->pipeline;

  // bind new mesh uniform to UBO and copy previous mesh uniform data
  mesh->ubo_slot = ubo_new_entry(scene->ubo, UBOType_Mesh);
  mesh_uniform_update(mesh);
  ubo_upload_entry(scene->ubo, UBOType_Mesh, &mesh->ubo_slot);

  ScenePipeline pipeline = scene_map_pipeline(mesh_pipeline);

  if (pipeline != ScenePipeline_Undefined) {

    // build mesh depending on pipeline and scene render mode
    scene_build_mesh(scene, mesh, pipeline);
    scene_add_mesh_core(scene, mesh, pipeline, layer, flag);

    return SceneStatus_Success;
  }

  return SceneStatus_UnvalidPipeline;
}

/**
   Add a list of mesh pointers (presumably from the scene mesh pool) to a
   pipeline. Meaning each meshes are going to be build depending on the pipeline
   and the current render mode.
 */
void scene_add_mesh_ref_list(Scene *scene, MeshRefList *list, const char *layer,
                             const SceneAddFlag flag) {
  for (size_t i = 0; i < list->length; i++)
    scene_add_mesh(scene, list->entries[i], layer, flag);
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
void scene_add_mesh_pipeline(Scene *scene, Mesh *mesh,
                             const ScenePipeline pipeline, const char *layer,
                             const SceneAddFlag flag) {

  mesh->ubo_slot = ubo_new_entry(scene->ubo, UBOType_Mesh);
  mesh_uniform_update(mesh);
  ubo_upload_entry(scene->ubo, UBOType_Mesh, &mesh->ubo_slot);

  scene_build_mesh(scene, mesh, pipeline);
  scene_add_mesh_core(scene, mesh, pipeline, layer, flag);
}

/**
   Add a list of mesh pointers (presumably from the scene mesh pool) to a
   pipeline. Meaning each meshes are going to be build depending on the pipeline
   and the current render mode.
 */
void scene_add_mesh_pipeline_ref_list(Scene *scene, MeshRefList *list,
                                      const ScenePipeline pipeline,
                                      const char *layer,
                                      const SceneAddFlag flag) {
  for (size_t i = 0; i < list->length; i++)
    scene_add_mesh_core(scene, list->entries[i], pipeline, layer, flag);
}
