#include "gizmo.h"
#include "../resources/loader/loader.mbin.h"
#include "../runtime/material/material.h"

void example_gizmo(Scene *scene) {

  Mesh *gizmo = scene_new_mesh(scene);
  Primitive mbin_primitive;
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/sphere.mbin",
      .primitive = &mbin_primitive,
  });

  mesh_create_primitive(gizmo, &(MeshCreatePrimitiveDescriptor){
                                   .primitive = mbin_primitive,
                                   .device = scene_device(scene),
                                   .queue = scene_queue(scene),
                                   .name = "gizmo",
                               });

  mesh_set_shader(gizmo, &(ShaderCreateDescriptor){
                             .path = SHADER_PATH_LINE,
                             .device = scene_device(scene),
                             .queue = scene_queue(scene),
                             .label = "gizmo shader",
                             .name = "gizmo shader",
                         });

  material_texture_double_sided(gizmo);

  mesh_translate(gizmo, (vec3){2.0f, 3.3f, 2.0f});

  scene_add_mesh(scene, gizmo, ScenePipeline_Fixed_Front, NULL);
}
