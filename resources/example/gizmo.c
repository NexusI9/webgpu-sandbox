#include "gizmo.h"

#include <cglm/types.h>
#include <stddef.h>

#include "resources/loader/loader.mbin.h"
#include "backend/std_pipeline/core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/pipeline/render.h"
#include "runtime/primitive/core.h"
#include "runtime/shader/core.h"
#include "runtime/scene/add.h"

void example_gizmo(Scene *scene) {

  Mesh *gizmo = scene_new_mesh(scene);
  Primitive mbin_primitive;
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/sphere.mbin",
      .primitive = &mbin_primitive,
  });

  mesh_create_primitive(gizmo, &(MeshCreatePrimitiveDescriptor){
                                   .primitive = &mbin_primitive,
                                   .device = scene_device(scene),
                                   .queue = scene_queue(scene),
                                   .name = "gizmo",
                               });

  mesh_shader_create_fixed(gizmo, &(ShaderCreateDescriptor){
                             .pipeline = std_render_pipeline(RenderPipelineType_Line),
                             .device = scene_device(scene),
                             .queue = scene_queue(scene),
                             .label = "gizmo shader",
                             .name = "gizmo shader",
                         });

  mesh_set_position(gizmo, (vec3){2.0f, 3.3f, 2.0f});

  scene_add_mesh_fixed(scene, gizmo, ScenePipeline_Fixed_Front, NULL);
}
