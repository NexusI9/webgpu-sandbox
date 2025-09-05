#include "add.h"
#include "../backend/ubo.h"
#include "../runtime/mesh/shader/shader.h"
#include "./editor/editor.h"
#include "./editor/object/object.h"
#include "./editor/selection/selection.h"
#include "build.h"
#include "core.h"
#include "editor/object/light/sun.h"
#include "editor/object/list/list.h"
#include "editor/object/probe/probe.h"
#include "editor/selection/core.h"
#include <stdint.h>
#include <stdio.h>

static inline void scene_add_seo(Scene *, SceneEditorObject *);

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
SceneEditorObject *scene_add_point_light(Scene *scene,
                                         PointLightDescriptor *desc,
                                         const LightShadow shadow) {

  PointLightListBase *base_list = &scene->lights.point.base;
  if (base_list->length == base_list->capacity) {
    VERBOSE_ERROR("Scene point light capacity reached maximum.");
    return 0;
  }

  // create sun light
  PointLight *new_light = &base_list->entries[base_list->length];
  light_point_create(new_light, desc);

  // transfert Light Uniform to SSBO
  ssbo_copy_entry(&scene->renderer.ssbo, SSBOType_PointLight,
                  &new_light->ssbo_slot[LightSSBOSlot_List]);

  // create mesh/gizmo
  SceneEditorObject *seo_light =
      seo_list_new_entry(scene_editor_object_list(scene));

  SEOCreateDescriptor seo_desc = {
      .camera = scene->active_camera,
      .viewport = &scene->viewport,
      .device = scene_device(scene),
      .queue = scene_queue(scene),
      .scene = scene,
      .target_list_index = 0,
  };

  if (shadow) {

    for (uint8_t i = 0; i < PROJECTION_VIEW_COUNT; i++)
      ssbo_copy_entry(&scene->renderer.ssbo, SSBOType_ViewProjection,
                      &new_light->ssbo_slot[LightSSBOSlot_View + i]);

    PointLightListShadow *shadow_list = &scene->lights.point.shadow;

    seo_desc.target_list_index = shadow_list->length;
    seo_light_point_shadow_create(seo_light, new_light, &seo_desc);

    light_list_point_shadow_insert(shadow_list, new_light);

    // recompute shadow map if render mode
    if (scene_renderer_draw_mode(&scene->renderer) ==
        SceneRendererDrawMode_Texture)
      shadow_map_draw_point_light(
          &(ShadowMapDrawPointLightDescriptor){
              .light = new_light,
              .pass = &scene->lights.point.shadow.pass,
              .device = scene_device(scene),
              .queue = scene_queue(scene),
              .texture_layer = shadow_list->length,
              .command_encoder = NULL,
          },
          SCENE_DEBUG_UNDEFINED);

  } else {
    seo_light_point_create(seo_light, new_light, &seo_desc);
  }

  // transfert gizmo mesh pointers to scene pipeline so they get rendered
  scene_add_seo(scene, seo_light);

  base_list->length++;

  ubo_update_entry(&scene->renderer.ubo, UBOField_PointLightCount,
                   (void *)&base_list->length);
  ubo_upload(&scene->renderer.ubo);

  return seo_light;
}

SceneEditorObject *scene_add_spot_light(Scene *scene, SpotLightDescriptor *desc,
                                        const LightShadow shadow) {

  SpotLightListBase *base_list = &scene->lights.spot.base;
  if (base_list->length == base_list->capacity) {
    VERBOSE_ERROR("Scene spot light capacity reached maximum.");
    return 0;
  }

  // create sun light
  SpotLight *new_light = &base_list->entries[base_list->length];
  light_spot_create(new_light, desc);

  // transfert Light Uniform to SSBO
  ssbo_copy_entry(&scene->renderer.ssbo, SSBOType_SpotLight,
                  &new_light->ssbo_slot[LightSSBOSlot_List]);

  // create mesh/gizmo
  SceneEditorObject *seo_light =
      seo_list_new_entry(scene_editor_object_list(scene));

  SEOCreateDescriptor seo_desc = {
      .camera = scene->active_camera,
      .viewport = &scene->viewport,
      .device = scene_device(scene),
      .queue = scene_queue(scene),
      .scene = scene,
      .target_list_index = 0,
  };

  if (shadow) {

    ssbo_copy_entry(&scene->renderer.ssbo, SSBOType_ViewProjection,
                    &new_light->ssbo_slot[LightSSBOSlot_View]);

    SpotLightListShadow *shadow_list = &scene->lights.spot.shadow;

    seo_desc.target_list_index = shadow_list->length;
    seo_light_spot_shadow_create(seo_light, new_light, &seo_desc);

    light_list_spot_shadow_insert(shadow_list, new_light);

    // recompute shadow map if render mode
    if (scene_renderer_draw_mode(&scene->renderer) ==
        SceneRendererDrawMode_Texture)
      shadow_map_draw_spot_light(
          &(ShadowMapDrawSpotLightDescriptor){
              .light = new_light,
              .pass = &shadow_list->pass,
              .device = scene_device(scene),
              .queue = scene_queue(scene),
              .texture_layer = shadow_list->length,
              .command_encoder = NULL,
          },
          SCENE_DEBUG_UNDEFINED);

  } else {

    seo_light_spot_create(seo_light, new_light, &seo_desc);
  }

  // transfert gizmo mesh pointers to scene pipeline so they get rendered
  scene_add_seo(scene, seo_light);

  base_list->length++;

  ubo_update_entry(&scene->renderer.ubo, UBOField_SpotLightCount,
                   (void *)&base_list->length);
  ubo_upload(&scene->renderer.ubo);

  return seo_light;
}

SceneEditorObject *scene_add_ambient_light(Scene *scene,
                                           AmbientLightDescriptor *desc) {

  AmbientLightList *list = &scene->lights.ambient;
  if (list->length == list->capacity) {
    VERBOSE_ERROR("Scene ambient light capacity reached maximum.");
    return 0;
  }

  // create sun light
  AmbientLight *new_light = &list->entries[list->length++];
  light_ambient_create(new_light, desc);

  // transfert Light Uniform to SSBO
  ssbo_copy_entry(&scene->renderer.ssbo, SSBOType_AmbientLight,
                  &new_light->ssbo_slot);

  // create mesh/gizmo
  SceneEditorObject *seo_light =
      seo_list_new_entry(scene_editor_object_list(scene));

  seo_light_ambient_create(seo_light, new_light,
                           &(SEOCreateDescriptor){
                               .camera = scene->active_camera,
                               .viewport = &scene->viewport,
                               .device = scene_device(scene),
                               .queue = scene_queue(scene),
                               .scene = scene,
                               .target_list_index = list->length - 1,
                           });

  // transfert gizmo mesh pointers to scene pipeline so they get rendered
  scene_add_seo(scene, seo_light);

  ubo_update_entry(&scene->renderer.ubo, UBOField_AmbientLightCount,
                   (void *)&list->length);
  ubo_upload(&scene->renderer.ubo);

  return seo_light;
}

SceneEditorObject *scene_add_sun_light(Scene *scene, SunLightDescriptor *desc,
                                       const LightShadow shadow) {

  SunLightListBase *base_list = &scene->lights.sun.base;
  if (base_list->length == base_list->capacity) {
    VERBOSE_ERROR("Scene sun light capacity reached maximum.");
    return NULL;
  }

  // create sun light
  SunLight *new_light = &base_list->entries[base_list->length];
  light_sun_create(new_light, desc);

  // transfert Light Uniform to SSBO
  ssbo_copy_entry(&scene->renderer.ssbo, SSBOType_SunLight,
                  &new_light->ssbo_slot[LightSSBOSlot_List]);

  // create mesh/gizmo
  SceneEditorObject *seo_light =
      seo_list_new_entry(scene_editor_object_list(scene));

  SEOCreateDescriptor seo_desc = {
      .camera = scene->active_camera,
      .viewport = &scene->viewport,
      .device = scene_device(scene),
      .queue = scene_queue(scene),
      .scene = scene,
      .target_list_index = 0,
  };

  if (shadow) {

    ssbo_copy_entry(&scene->renderer.ssbo, SSBOType_ViewProjection,
                    &new_light->ssbo_slot[LightSSBOSlot_View]);

    SunLightListShadow *shadow_list = &scene->lights.sun.shadow;

    seo_desc.target_list_index = shadow_list->length;

    seo_light_sun_shadow_create(seo_light, new_light, &seo_desc);

    light_list_sun_shadow_insert(shadow_list, new_light);

    // recompute shadow map if render mode
    if (scene_renderer_draw_mode(&scene->renderer) ==
        SceneRendererDrawMode_Texture)
      shadow_map_draw_sun_light(
          &(ShadowMapDrawSunLightDescriptor){
              .light = new_light,
              .pass = &scene->lights.spot.shadow.pass,
              .device = scene_device(scene),
              .queue = scene_queue(scene),
              .texture_layer =
                  scene->lights.spot.shadow.length + seo_desc.target_list_index,
              .command_encoder = NULL,
          },
          SCENE_DEBUG_UNDEFINED);

  } else {
    seo_light_sun_create(seo_light, new_light, &seo_desc);
  }

  // transfert gizmo mesh pointers to scene pipeline so they get rendered
  scene_add_seo(scene, seo_light);

  base_list->length++;

  ubo_update_entry(&scene->renderer.ubo, UBOField_SunLightCount,
                   (void *)&base_list->length);
  ubo_upload(&scene->renderer.ubo);

  return seo_light;
}

/**
   "Create" a new uninitialized camera in the scene camera list and return the
   newly created item's pointer.

        ScenePool
       .---------.
       |   ...   |
       |---------|              SEO<T>
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
SceneEditorObject *scene_add_camera(Scene *scene,
                                    const CameraCreateDescriptor *desc) {

  // init scene camera
  Camera *new_cam = camera_list_new_camera(&scene->cameras);
  camera_create(new_cam, desc);

  // create gizmo
  SceneEditorObject *seo_cam =
      seo_list_new_entry(scene_editor_object_list(scene));

  seo_camera_create(seo_cam, new_cam,
                    &(SEOCreateDescriptor){
                        .camera = scene->active_camera,
                        .viewport = &scene->viewport,
                        .device = scene_device(scene),
                        .queue = scene_queue(scene),
                        .scene = scene,
                        .target_list_index = scene->cameras.length - 1,
                    });

  // transfert gizmo mesh pointers to scene pipeline so they get rendered
  scene_add_seo(scene, seo_cam);

  return seo_cam;
}

/**
   Compared to casual meshes, Scene Editor Objects (lights/ camera) need to
   follow a more specific path when it comes to be added to the scene and
   especially how they are handled for selection.

   SEO are segmented into:
   1. Meshes list: a visual helper/ representaiton of the entity
   2. Target: the actual data

   As a result in order to add them to the scene we need to add their meshes
   list in the scene pool, but need to add them in a different "branch" of the
   selection system.

   We use the below function to do such operation.
 */
void scene_add_seo(Scene *scene, SceneEditorObject *seo) {

  for (size_t i = 0; i < seo->meshes.length; i++) {
    Mesh *mesh = seo->meshes.entries[i].mesh;
    // build mesh depending on pipeline and scene render mode
    scene_build_mesh(scene, mesh, ScenePipeline_Fixed);
    mesh_ref_list_insert(scene_pipeline(scene, ScenePipeline_Fixed), mesh);

    // add the SEO into the right selection branch/ filter and link the SEO as
    // extra
    scene_selection_add_mesh(&scene->editor.selection, mesh, seo,
                             SceneSelectionType_SEO);
  }
}

SceneEditorObject *
scene_add_probe_reflection_grid(Scene *scene,
                                ProbeReflectionGridDescriptor *desc) {

  ProbeReflectionGrid *new_grid =
      probe_reflection_grid_list_new_entry(&scene->probes_reflection);

  probe_reflection_grid_create(new_grid, desc);

  // create scene object
  SceneEditorObject *seo_grid =
      seo_list_new_entry(scene_editor_object_list(scene));

  seo_probe_reflection_grid_create(seo_grid, new_grid,
                                   &(SEOCreateDescriptor){
                                       .camera = scene->active_camera,
                                       .viewport = &scene->viewport,
                                       .device = scene_device(scene),
                                       .queue = scene_queue(scene),
                                       .scene = scene,
                                       .target_list_index = 0,
                                   });

  // add probes to ssbo list
  for (uint16_t i = 0; i < new_grid->probes.length; i++) {
    ProbeReflection *probe = &new_grid->probes.entries[i];
    SSBOManager *ssbo = &scene->renderer.ssbo;

    // add to pos/radius list
    ssbo_copy_entry(ssbo, SSBOType_ProbeGridReflection,
                    &probe->ssbo_slot[ProbeReflectionSSBOField_List]);

    // add each views
    for (uint8_t v = 0; v < PROBE_REFLECTION_VIEW_COUNT; v++) {
      ssbo_copy_entry(ssbo, SSBOType_Camera,
                      &probe->ssbo_slot[ProbeReflectionSSBOField_View + v]);
    }
  }

  // update UBO for probe count
  size_t probe_count =
      probe_reflection_grid_list_probe_count(&scene->probes_reflection);
  ubo_update_entry(&scene->renderer.ubo, UBOField_ProbeReflectionGridCount,
                   (void *)&probe_count);

  ubo_upload(&scene->renderer.ubo);

  // transfert gizmo mesh pointers to scene pipeline so they get rendered
  scene_add_seo(scene, seo_grid);

  return seo_grid;
}

SceneEditorObject *
scene_add_probe_reflection_plane(Scene *scene,
                                 ProbeReflectionPlaneDescriptor *desc) {

  // add draw callback if first probe
  if (scene->planes_reflection.length == 0)
    scene_renderer_add_draw_callback(&scene->renderer,
                                     probe_reflection_plane_list_draw_callback,
                                     (void *)&scene->planes_reflection);

  ProbeReflectionPlane *probe =
      probe_reflection_plane_list_new_entry(&scene->planes_reflection);

  probe_reflection_plane_create(probe, desc);

  // create scene object
  SceneEditorObject *seo_grid =
      seo_list_new_entry(scene_editor_object_list(scene));

  probe_reflection_plane_create(probe, desc);

  seo_probe_reflection_plane_create(
      seo_grid, probe,
      &(SEOCreateDescriptor){
          .camera = scene->active_camera,
          .viewport = &scene->viewport,
          .device = scene_device(scene),
          .queue = scene_queue(scene),
          .scene = scene,
          .target_list_index = SCENE_EDITOR_OBJECT_TARGET_UNDEFINED,
      });

  // add probes to ssbo list
  SSBOManager *ssbo = &scene->renderer.ssbo;

  // add each view
  ssbo_copy_entry(ssbo, SSBOType_Camera,
                  &probe->ssbo_slot[ProbeReflectionSSBOField_View]);

  // update uniform to update camera/view ssbo id
  probe_reflection_plane_update_uniform(probe);

  // add to pos/radius list
  ssbo_copy_entry(ssbo, SSBOType_ProbePlaneReflection,
                  &probe->ssbo_slot[ProbeReflectionSSBOField_List]);

  // update UBO for probe count
  ubo_update_entry(&scene->renderer.ubo, UBOField_ProbeReflectionPlaneCount,
                   (void *)&scene->planes_reflection.length);

  ubo_upload(&scene->renderer.ubo);

  // transfert gizmo mesh pointers to scene pipeline so they get rendered
  scene_add_seo(scene, seo_grid);

  return seo_grid;
}

/**
  ▗▖  ▗▖▗▄▄▄▖ ▗▄▄▖▗▖ ▗▖
  ▐▛▚▞▜▌▐▌   ▐▌   ▐▌ ▐▌
  ▐▌  ▐▌▐▛▀▀▘ ▝▀▚▖▐▛▀▜▌
  ▐▌  ▐▌▐▙▄▄▖▗▄▄▞▘▐▌ ▐▌

 */

Mesh *scene_new_mesh(Scene *scene) {
  return mesh_list_new_mesh(&scene->meshes);
}

static void scene_add_mesh_any(Scene *, Mesh *, const ScenePipeline,
                               const char *);

void scene_add_mesh_any(Scene *scene, Mesh *mesh, const ScenePipeline pipeline,
                        const char *layer) {

  // add to scene layers ('Default' layer if NULL)
  if (layer == NULL)
    layer = SCENE_LAYER_DEFAULT;
  scene_layer_set_insert_mesh(&scene->layers, layer, mesh);

  // add mesh pointer to the right pipeline
  mesh_ref_list_insert(scene_pipeline(scene, pipeline), mesh);

  // Update Shadow maps if added to Dynamic_Lit pipeline
  if (pipeline == ScenePipeline_Dynamic_LitShadow &&
      scene->renderer.draw.mode == SceneRendererDrawMode_Texture) {
    shadow_map_draw_all(
        &(ShadowMapDrawAllDescriptor){
            .device = scene_device(scene),
            .queue = scene_queue(scene),
            .mesh_list = scene_pipeline(scene, ScenePipeline_Dynamic_LitShadow),
            .lights = &scene->lights,
        },
        SCENE_DEBUG_UNDEFINED);

    // EDITORONLY
    // add mesh to selection shadow
    scene_selection_add_mesh(&scene->editor.selection, mesh, NULL,
                             SceneSelectionType_MeshShadow);

  } else {
    // EDITORONLY
    // add mesh to selection
    scene_selection_add_mesh(&scene->editor.selection, mesh, NULL,
                             SceneSelectionType_Mesh);
  }
}

/**
   Add mesh to the dynamic pipeline.
   Depending on the mesh current texture pipeline, it will either dispatch the
   mesh to the unlit or lit pipeline.
   Basically if mesh texture shader has PBR pipeline it goes to the lit, if it
   has Unlit pieline it goes to the unlit.

   To add a mesh to the fixed pipelines (ex: Gizmo, Scene Editor Objects), the
   scene_add_mesh_fixed dedicated function shall be used.

   The below function is designed for "common usage", meaning on a daily basis,
   one will add dynamic assets to the scene, compared to the fixed elements
   which are only used by the editor itself.
 */
void scene_add_mesh(Scene *scene, Mesh *mesh, const char *layer) {

  // dispatch mesh based on their global pipeline address (lit by default)
  ScenePipeline pipeline = ScenePipeline_Dynamic_LitShadow;

  // TODO: find a cleaner way to define if mesh is Shadowed or not.. the
  // overallx dispatch is unclear.
  const Pipeline *mesh_pipeline =
      mesh_shader(mesh, MeshShader_Texture)->pipeline;

  if (mesh_pipeline == std_pipeline(PipelineType_Unlit) ||
      mesh_pipeline == std_pipeline(PipelineType_GlassBox) ||
      mesh_pipeline == std_pipeline(PipelineType_GlassProbeGrid) ||
      mesh_pipeline == std_pipeline(PipelineType_GlassProbePlane))
    pipeline = ScenePipeline_Dynamic_Unlit;

  // build mesh depending on pipeline and scene render mode
  scene_build_mesh(scene, mesh, pipeline);

  scene_add_mesh_any(scene, mesh, pipeline, layer);
}

/**
   Add a list of mesh pointers (presumably from the scene mesh pool) to a
   pipeline. Meaning each meshes are going to be build depending on the pipeline
   and the current render mode.
 */
void scene_add_mesh_ref_list(Scene *scene, MeshRefList *list,
                             const char *layer) {
  for (size_t i = 0; i < list->length; i++)
    scene_add_mesh(scene, list->entries[i], layer);
}

void scene_add_mesh_fixed(Scene *scene, Mesh *mesh,
                          const ScenePipeline pipeline, const char *layer) {

  scene_build_mesh(scene, mesh, pipeline);

  scene_add_mesh_any(scene, mesh, pipeline, layer);
}

/**
   Add a list of mesh pointers (presumably from the scene mesh pool) to a
   pipeline. Meaning each meshes are going to be build depending on the pipeline
   and the current render mode.
 */
void scene_add_mesh_fixed_ref_list(Scene *scene, MeshRefList *list,
                                   const ScenePipeline pipeline,
                                   const char *layer) {
  for (size_t i = 0; i < list->length; i++)
    scene_add_mesh_any(scene, list->entries[i], pipeline, layer);
}
