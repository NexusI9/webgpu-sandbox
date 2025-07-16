#include "add.h"

static MeshRefList *scene_layer_gizmo(Scene *);

GizmoPointLight *scene_add_point_light(Scene *scene,
                                       PointLightDescriptor *desc) {

  PointLightList *list = &scene->lights.point;
  if (list->length == list->capacity) {
    perror("Scene point light capacity reached maximum");
    return 0;
  }

  // create sun light
  PointLight *new_light = &list->entries[list->length++];
  light_create_point(new_light, desc);

  // create mesh/gizmo
  GizmoPointLight *gizmo_light = gizmo_list_new_point_light(&scene->gizmo);

  gizmo_light_point_create(gizmo_light, new_light,
                           &(GizmoCreateDescriptor){
                               .camera = scene->active_camera,
                               .viewport = &scene->viewport,
                               .device = scene->device,
                               .queue = scene->queue,
                               .list = scene_mesh_list(scene),
                           });


  // transfert gizmo mesh pointers to render_list so they get rendered
  MeshRefList *render_list = scene_layer_gizmo(scene);
  mesh_reference_list_transfert(&gizmo_light->meshes, render_list);


  return gizmo_light;
}

GizmoSpotLight *scene_add_spot_light(Scene *scene, SpotLightDescriptor *desc) {

  SpotLightList *list = &scene->lights.spot;
  if (list->length == list->capacity) {
    perror("Scene spot light capacity reached maximum");
    return 0;
  }

  // create sun light
  SpotLight *new_light = &list->entries[list->length++];
  light_create_spot(new_light, desc);

  // create mesh/gizmo
  GizmoSpotLight *gizmo_light = gizmo_list_new_spot_light(&scene->gizmo);

  gizmo_light_spot_create(gizmo_light, new_light,
                          &(GizmoCreateDescriptor){
                              .camera = scene->active_camera,
                              .viewport = &scene->viewport,
                              .device = scene->device,
                              .queue = scene->queue,
                              .list = scene_mesh_list(scene),
                          });

  // transfert gizmo mesh pointers to render_list so they get rendered
  MeshRefList *render_list = scene_layer_gizmo(scene);
  mesh_reference_list_transfert(&gizmo_light->meshes, render_list);

  return gizmo_light;
}

GizmoAmbientLight *scene_add_ambient_light(Scene *scene,
                                           AmbientLightDescriptor *desc) {

  AmbientLightList *list = &scene->lights.ambient;
  if (list->length == list->capacity) {
    perror("Scene ambient light capacity reached maximum");
    return 0;
  }

  // create sun light
  AmbientLight *new_light = &list->entries[list->length++];
  light_create_ambient(new_light, desc);

  // create mesh/gizmo
  GizmoAmbientLight *gizmo_light = gizmo_list_new_ambient_light(&scene->gizmo);

  gizmo_light_ambient_create(gizmo_light, new_light,
                             &(GizmoCreateDescriptor){
                                 .camera = scene->active_camera,
                                 .viewport = &scene->viewport,
                                 .device = scene->device,
                                 .queue = scene->queue,
                                 .list = scene_mesh_list(scene),
                             });

  // transfert gizmo mesh pointers to render_list so they get rendered
  MeshRefList *render_list = scene_layer_gizmo(scene);
  mesh_reference_list_transfert(&gizmo_light->meshes, render_list);

  return gizmo_light;
}

GizmoSunLight *scene_add_sun_light(Scene *scene, SunLightDescriptor *desc) {

  SunLightList *list = &scene->lights.sun;
  if (list->length == list->capacity) {
    perror("Scene sun light capacity reached maximum");
    return NULL;
  }

  // create sun light
  SunLight *new_light = &list->entries[list->length++];
  light_create_sun(new_light, desc);

  // create mesh/gizmo
  GizmoSunLight *gizmo_light = gizmo_list_new_sun_light(&scene->gizmo);

  gizmo_light_sun_create(gizmo_light, new_light,
                         &(GizmoCreateDescriptor){
                             .camera = scene->active_camera,
                             .viewport = &scene->viewport,
                             .device = scene->device,
                             .queue = scene->queue,
                             .list = scene_mesh_list(scene),
                         });

  // transfert gizmo mesh pointers to render_list so they get rendered
  MeshRefList *render_list = scene_layer_gizmo(scene);
  mesh_reference_list_transfert(&gizmo_light->meshes, render_list);

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
  GizmoCamera *gizmo_cam = gizmo_list_new_camera(&scene->gizmo);

  gizmo_camera_create(gizmo_cam, new_cam,
                      &(GizmoCreateDescriptor){
                          .camera = scene->active_camera,
                          .viewport = &scene->viewport,
                          .device = scene->device,
                          .queue = scene->queue,
                          .list = scene_mesh_list(scene),
                      });

  // transfert gizmo mesh pointers to render_list
  MeshRefList *render_list = scene_layer_gizmo(scene);
  mesh_reference_list_transfert(&gizmo_cam->meshes, render_list);

  return gizmo_cam;
}

/**
   Return pointer to mesh gizmo layer ("Fixed" layer)
 */
MeshRefList *scene_layer_gizmo(Scene *scene) { return &scene->pipelines.fixed; }
