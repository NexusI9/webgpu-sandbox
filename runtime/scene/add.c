#include "add.h"
#include "./editor/editor.h"
#include "build.h"
#include "core.h"

static ScenePipeline gizmo_pipeline = ScenePipeline_Fixed;

GizmoPointLight *scene_add_point_light(Scene *scene,
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
  GizmoPointLight *gizmo_light =
      gizmo_list_new_point_light(scene_editor_gizmo_list(scene));

  gizmo_light_point_create(gizmo_light, new_light,
                           &(GizmoCreateDescriptor){
                               .camera = scene->active_camera,
                               .viewport = &scene->viewport,
                               .device = scene_device(scene),
                               .queue = scene_queue(scene),
                               .list = scene_mesh_list(scene),
                           });

  // transfert gizmo mesh pointers to scene pipeline so they get rendered
  scene_add_mesh_ref_list(scene, &gizmo_light->meshes, gizmo_pipeline,
                                NULL);

  return gizmo_light;
}

GizmoSpotLight *scene_add_spot_light(Scene *scene, SpotLightDescriptor *desc) {

  SpotLightList *list = &scene->lights.spot;
  if (list->length == list->capacity) {
    VERBOSE_ERROR("Scene spot light capacity reached maximum.");
    return 0;
  }

  // create sun light
  SpotLight *new_light = &list->entries[list->length++];
  light_create_spot(new_light, desc);

  // create mesh/gizmo
  GizmoSpotLight *gizmo_light =
      gizmo_list_new_spot_light(scene_editor_gizmo_list(scene));

  gizmo_light_spot_create(gizmo_light, new_light,
                          &(GizmoCreateDescriptor){
                              .camera = scene->active_camera,
                              .viewport = &scene->viewport,
                              .device = scene_device(scene),
                              .queue = scene_queue(scene),
                              .list = scene_mesh_list(scene),
                          });

  // transfert gizmo mesh pointers to scene pipeline so they get rendered
  scene_add_mesh_ref_list(scene, &gizmo_light->meshes, gizmo_pipeline,
                                NULL);

  return gizmo_light;
}

GizmoAmbientLight *scene_add_ambient_light(Scene *scene,
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
  GizmoAmbientLight *gizmo_light =
      gizmo_list_new_ambient_light(scene_editor_gizmo_list(scene));

  gizmo_light_ambient_create(gizmo_light, new_light,
                             &(GizmoCreateDescriptor){
                                 .camera = scene->active_camera,
                                 .viewport = &scene->viewport,
                                 .device = scene_device(scene),
                                 .queue = scene_queue(scene),
                                 .list = scene_mesh_list(scene),
                             });

  // transfert gizmo mesh pointers to scene pipeline so they get rendered
  scene_add_mesh_ref_list(scene, &gizmo_light->meshes, gizmo_pipeline,
                                NULL);

  return gizmo_light;
}

GizmoSunLight *scene_add_sun_light(Scene *scene, SunLightDescriptor *desc) {

  SunLightList *list = &scene->lights.sun;
  if (list->length == list->capacity) {
    VERBOSE_ERROR("Scene sun light capacity reached maximum.");
    return NULL;
  }

  // create sun light
  SunLight *new_light = &list->entries[list->length++];
  light_create_sun(new_light, desc);

  // create mesh/gizmo
  GizmoSunLight *gizmo_light =
      gizmo_list_new_sun_light(scene_editor_gizmo_list(scene));

  gizmo_light_sun_create(gizmo_light, new_light,
                         &(GizmoCreateDescriptor){
                             .camera = scene->active_camera,
                             .viewport = &scene->viewport,
                             .device = scene_device(scene),
                             .queue = scene_queue(scene),
                             .list = scene_mesh_list(scene),
                         });

  // transfert gizmo mesh pointers to scene pipeline so they get rendered
  scene_add_mesh_ref_list(scene, &gizmo_light->meshes, gizmo_pipeline,
                                NULL);

  return gizmo_light;
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
GizmoCamera *scene_add_camera(Scene *scene,
                              const CameraCreateDescriptor *desc) {

  // init scene camera
  Camera *new_cam = camera_list_new_camera(&scene->cameras);
  camera_create(new_cam, desc);

  // create gizmo
  GizmoCamera *gizmo_cam =
      gizmo_list_new_camera(scene_editor_gizmo_list(scene));

  gizmo_camera_create(gizmo_cam, new_cam,
                      &(GizmoCreateDescriptor){
                          .camera = scene->active_camera,
                          .viewport = &scene->viewport,
                          .device = scene_device(scene),
                          .queue = scene_queue(scene),
                          .list = scene_mesh_list(scene),
                      });

  // transfert gizmo mesh pointers to scene pipeline so they get rendered
  scene_add_mesh_ref_list(scene, &gizmo_cam->meshes, gizmo_pipeline,
                                NULL);

  return gizmo_cam;
}

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
}

/**
   Add a list of mesh pointers (presumably from the scene mesh pool) to a
   pipeline. Meaning each meshes are going to be build depending on the pipeline
   and the current render mode.
 */
void scene_add_mesh_ref_list(Scene *scene, MeshRefList *list,
                                   const ScenePipeline mode,
                                   const char *layer) {
  for (size_t i = 0; i < list->length; i++)
    scene_add_mesh(scene, list->entries[i], mode, layer);
}
