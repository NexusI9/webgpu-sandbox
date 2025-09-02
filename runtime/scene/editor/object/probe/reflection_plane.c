#include "reflection_plane.h"
#include "../builder/builder.h"
#include "../resources/loader/loader.mbin.h"
#include "../runtime/scene/editor/object/object.h"
#include "../runtime/scene/scene.h"
#include <stdint.h>

void seo_probe_reflection_plane_create(SceneEditorObject *seo,
                                       ProbeReflectionPlane *probe,
                                       const SEOCreateDescriptor *desc) {

  seo->scene = desc->scene;

  const uint16_t seo_mesh_count = 1;
  seo_mesh_list_create(&seo->meshes, seo_mesh_count);

  SceneEditorObjectMesh *probe_bound = seo_mesh_list_new_entry(&seo->meshes);

  probe_bound->mesh = scene_new_mesh(desc->scene);
  probe_bound->target = probe;
  probe_bound->target_list_index = desc->target_list_index; // necessary ?

  Primitive primitive;
  // TODO: cache MBIN
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/plane.mbin",
      .primitive = &primitive,
  });

  SEOCreateWireframeDescriptor wireframe_desc = {
      .color = &(color){1.0f, 0.0f, 0.0f, 1.0f},
      .device = desc->device,
      .queue = desc->queue,
      .index = &primitive.index,
      .vertex = &primitive.vertex,
      .name = "seo probe reflection plane",
      .thickness = SEO_WIREFRAME_LINE_THICKNESS,
  };

  seo_create_wireframe(probe_bound->mesh, &wireframe_desc);

  mesh_set_scale(probe_bound->mesh, probe->scale);
  mesh_set_position(probe_bound->mesh, probe->position);

  probe_bound->transform_callback[GizmoMode_Position] =
      seo_probe_reflection_plane_set_position;
  probe_bound->transform_callback[GizmoMode_Rotation] =
      seo_probe_reflection_plane_set_rotation;
  probe_bound->transform_callback[GizmoMode_Scale] =
      seo_probe_reflection_plane_set_scale;

  seo->origin = probe_bound->mesh;
}

void seo_probe_reflection_plane_set_position(SEOTransformCallback *desc) {

  mesh_set_position(desc->mesh->mesh, desc->offset);

  ProbeReflectionPlane *probe = (ProbeReflectionPlane *)desc->mesh->target;
  glm_vec3_copy(desc->mesh->mesh->position, probe->position);

  // update uniform cpu side
  probe_reflection_plane_update_uniform(probe);

  // add to upload queue
  ssbo_update_queue_insert(&desc->seo->scene->renderer.ssbo,
                           SSBOType_ProbePlaneReflection,
                           probe->ssbo_slot[ProbeReflectionSSBOField_List].id);

  // update view cpu side
  probe_reflection_plane_update_view(probe);

  // add to upload queue
  ssbo_update_queue_insert(&desc->seo->scene->renderer.ssbo,
                           SSBOType_ViewProbeReflection,
                           probe->ssbo_slot[ProbeReflectionSSBOField_View].id);
}

void seo_probe_reflection_plane_set_rotation(SEOTransformCallback *desc) {}

void seo_probe_reflection_plane_set_scale(SEOTransformCallback *desc) {

  // print_vec3(desc->offset);
}
