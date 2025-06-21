#include "gizmo.h"

void example_gizmo(Scene* scene, Renderer* renderer) {

  Mesh *gizmo = scene_new_mesh_fixed(scene);
  Primitive mbin_primitive;
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/sphere.mbin",
      .primitive = &mbin_primitive,
  });

  mesh_create_primitive(gizmo, &(MeshCreatePrimitiveDescriptor){
                                   .primitive = mbin_primitive,
                                   .device = renderer_device(renderer),
                                   .queue = renderer_queue(renderer),
                                   .name = "gizmo",
                               });

  mesh_set_shader(gizmo, &(ShaderCreateDescriptor){
                             .path = SHADER_PATH_LINE,
                             .device = renderer_device(renderer),
                             .queue = renderer_queue(renderer),
                             .label = "gizmo shader",
                             .name = "gizmo shader",
                         });

  material_texture_double_sided(gizmo);

  mesh_translate(gizmo, (vec3){2.0f, 3.3f, 2.0f});
}
