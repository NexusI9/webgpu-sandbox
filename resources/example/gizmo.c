#include "gizmo.h"

#include <cglm/types.h>
#include <stddef.h>

#include "backend/context.h"
#include "backend/std_pipeline/core.h"
#include "resources/loader/loader.mbin.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/pipeline/render.h"
#include "runtime/primitive/core.h"
#include "runtime/scene/add.h"
#include "runtime/shader/core.h"

void example_gizmo(Scene *scene) {

  Mesh *gizmo = scene_new_mesh(scene);
  Primitive mbin_primitive;
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/sphere.mbin",
      .primitive = &mbin_primitive,
  });

  mesh_create_primitive(gizmo, &(MeshCreatePrimitiveDescriptor){
                                   .primitive = &mbin_primitive,
                                   .name = "gizmo",
                               });

  mesh_shader_create_fixed(
      gizmo, &(ShaderCreateDescriptor){
                 .pipeline = std_render_pipeline(RenderPipelineType_Line),
                 .name = "gizmo shader",
             });

  mesh_set_position(gizmo, (vec3){2.0f, 3.3f, 2.0f});

  scene_add_mesh_pipeline(scene, gizmo, ScenePipeline_Fixed_Front, NULL,
                       SceneAddFlag_None);
}
