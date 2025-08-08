#include "gizmo.h"
#include "../backend/renderer/scene/scene.h"
#include "../resources/loader/loader.mbin.h"
#include "../runtime/mesh/shader/shader.h"

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

  mesh_shader_create_fixed(gizmo, &(ShaderCreateDescriptor){
                             .pipeline = std_pipeline(PipelineType_Line),
                             .device = scene_device(scene),
                             .queue = scene_queue(scene),
                             .label = "gizmo shader",
                             .name = "gizmo shader",
                         });

  mesh_translate(gizmo, (vec3){2.0f, 3.3f, 2.0f});

  scene_add_mesh_fixed(scene, gizmo, ScenePipeline_Fixed_Front, NULL);
}
