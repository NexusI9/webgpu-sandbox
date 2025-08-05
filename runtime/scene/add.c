#include "add.h"
#include "./editor/editor.h"
#include "./editor/object/object.h"
#include "./editor/selection/selection.h"
#include "build.h"
#include "core.h"
#include "editor/object/list/list.h"
#include "editor/selection/core.h"

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
                                         PointLightDescriptor *desc) {

  PointLightList *list = &scene->lights.point;
  if (list->length == list->capacity) {
    VERBOSE_ERROR("Scene point light capacity reached maximum.");
    return 0;
  }

  // create sun light
  PointLight *new_light = &list->entries[list->length++];
  light_create_point(new_light, desc);

  // create mesh/gizmo
  SceneEditorObject *seo_light =
      seo_list_new_entry(scene_editor_object_list(scene));

  seo_light_point_create(seo_light, new_light,
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

  return seo_light;
}

SceneEditorObject *scene_add_spot_light(Scene *scene,
                                        SpotLightDescriptor *desc) {

  SpotLightList *list = &scene->lights.spot;
  if (list->length == list->capacity) {
    VERBOSE_ERROR("Scene spot light capacity reached maximum.");
    return 0;
  }

  // create sun light
  SpotLight *new_light = &list->entries[list->length++];
  light_create_spot(new_light, desc);

  // create mesh/gizmo
  SceneEditorObject *seo_light =
      seo_list_new_entry(scene_editor_object_list(scene));

  seo_light_spot_create(seo_light, new_light,
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
  light_create_ambient(new_light, desc);

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

  return seo_light;
}

SceneEditorObject *scene_add_sun_light(Scene *scene, SunLightDescriptor *desc) {

  SunLightList *list = &scene->lights.sun;
  if (list->length == list->capacity) {
    VERBOSE_ERROR("Scene sun light capacity reached maximum.");
    return NULL;
  }

  // create sun light
  SunLight *new_light = &list->entries[list->length++];
  light_create_sun(new_light, desc);

  // create mesh/gizmo
  SceneEditorObject *seo_light =
      seo_list_new_entry(scene_editor_object_list(scene));

  seo_light_sun_create(seo_light, new_light,
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
  return seo_light;
}

/**
   "Create" a new uninitialized camera in the scene camera list and return the
   newly created item's pointer.

      ScenePool<GizmoT>
       .---------.
       |   ...   |
       |---------|             <GizmoT>
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
    Mesh *mesh = seo->meshes.entries[i];
    // build mesh depending on pipeline and scene render mode
    scene_build_mesh(scene, mesh, ScenePipeline_Fixed);
    mesh_ref_list_insert(&scene->pipelines[ScenePipeline_Fixed], mesh);
  }

  // add the SEO into the right selection branch/ filter and link the SEO as
  // extra
  scene_selection_add_mesh_ref_list(&scene->editor.selection, &seo->meshes, seo,
                                    SceneSelectionType_SEO);
}

/**
  ▗▖  ▗▖▗▄▄▄▖ ▗▄▄▖▗▖ ▗▖
  ▐▛▚▞▜▌▐▌   ▐▌   ▐▌ ▐▌
  ▐▌  ▐▌▐▛▀▀▘ ▝▀▚▖▐▛▀▜▌
  ▐▌  ▐▌▐▙▄▄▖▗▄▄▞▘▐▌ ▐▌

 */

Mesh *scene_new_mesh(Scene *scene) {
  Mesh *new_mesh = mesh_list_new_mesh(&scene->meshes);

  return new_mesh;
}

/**
 Return the new mesh pointer from the global array and push the new pointer to
 the right scene layer.
  1. first create new mesh in the scene pool
  2. add the reference to the relative mesh ref list
 */
void scene_add_mesh(Scene *scene, Mesh *mesh, const ScenePipeline pipeline,
                    const char *layer) {

  // build mesh depending on pipeline and scene render mode
  scene_build_mesh(scene, mesh, pipeline);

  // add to scene layers ('Default' layer if NULL)
  if (layer == NULL)
    layer = SCENE_LAYER_DEFAULT;
  scene_layer_set_insert_mesh(&scene->layers, layer, mesh);

  // add mesh pointer to the right pipeline
  mesh_ref_list_insert(&scene->pipelines[pipeline], mesh);

  // Update Shadow maps if added to Dynamic_Lit pipeline
  if (pipeline == ScenePipeline_Dynamic_Lit)
    shadow_map_draw_all(&(ShadowMapDrawAllDescriptor){
        .device = scene_device(scene),
        .queue = scene_queue(scene),
        .mesh_list = &scene->pipelines[ScenePipeline_Dynamic_Lit],
        .lights = &scene->lights,
    });

  // EDITORONLY
  // add mesh to selection
  scene_selection_add_mesh(&scene->editor.selection, mesh, NULL,
                           SceneSelectionType_Mesh);
}

/**
   Add a list of mesh pointers (presumably from the scene mesh pool) to a
   pipeline. Meaning each meshes are going to be build depending on the pipeline
   and the current render mode.
 */
void scene_add_mesh_ref_list(Scene *scene, MeshRefList *list,
                             const ScenePipeline mode, const char *layer) {
  for (size_t i = 0; i < list->length; i++)
    scene_add_mesh(scene, list->entries[i], mode, layer);
}
