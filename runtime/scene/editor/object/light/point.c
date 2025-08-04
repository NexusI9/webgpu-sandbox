#include "./point.h"
#include "../builder/builder.h"
#include "../resources/loader/loader.mbin.h"

/**
   Insert Point light gizmo mesh to the list
 */
void seo_light_point_create(SceneEditorObject *seo, PointLight *light,
                            const GizmoCreateDescriptor *desc) {

  // define target
  seo->target = light;

  // define mesh
  size_t gizmo_mesh_count = 1;
  mesh_ref_list_create(&seo->meshes, gizmo_mesh_count);

  // get new mesh pointer from main mesh list
  Mesh *icon = mesh_list_new_mesh(desc->list);
  const char *texture_path = "./resources/assets/texture/ui/light-point.png";

  // create gizmo mesh
  seo_create_billboard(icon, &(SEOCreateBillboardDescriptor){
                                 .texture_path = texture_path,
                                 .device = desc->device,
                                 .queue = desc->queue,
                                 .position = &light->position,
                                 .scale = &SEO_BILLBOARD_SCALE,
                             });

  // store mesh pointer in gizmo ref list
  mesh_ref_list_insert(&seo->meshes, icon);

  // create sphere
  /*DELELTEME: Mesh *sphere = mesh_list_new_mesh(desc->list);
  Primitive sphere_primitive;
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .primitive = &sphere_primitive,
      .path = "./resources/assets/mbin/sphere.mbin",
  });

  gizmo_create_wireframe(sphere, &(GizmoCreateWireframeDescriptor){
                                     .color = &(vec3){0.4f, 0.8f, 1.0f},
                                     .device = desc->device,
                                     .queue = desc->queue,
                                     .name = "Gizmo spot light sphere",
                                     .index = &sphere_primitive.index,
                                     .vertex = &sphere_primitive.vertex,
                                     .thickness = 0.005f,
                                 });

  // scale sphere to point far point
  mesh_scale(sphere, (vec3){light->far, light->far, light->far});

  mesh_ref_list_insert(&gizmo->meshes, sphere);*/

  // set callback
  seo->transform_callback[GizmoTransformMode_Translate] =
      seo_light_point_translate;
  seo->transform_callback[GizmoTransformMode_Rotate] = seo_light_point_rotate;
  seo->transform_callback[GizmoTransformMode_Scale] = seo_light_point_scale;
}

void seo_light_point_translate(SceneEditorObject *seo, vec3 value) {

  mesh_ref_list_translate(&seo->meshes, value);
}

void seo_light_point_rotate(SceneEditorObject *seo, vec3 value) {}

void seo_light_point_scale(SceneEditorObject *seo, vec3 value) {}
