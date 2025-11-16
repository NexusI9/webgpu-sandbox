#include "gizmo.h"

#include <cglm/types.h>
#include <stddef.h>

#include "backend/context.h"
#include "backend/renderer/batch.h"
#include "backend/renderer/core.h"
#include "backend/resource_manager.h"
#include "backend/std_pipeline/core.h"
#include "resources/loader/loader.mbin.h"
#include "runtime/engine/add.h"
#include "runtime/engine/core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/pipeline/render.h"
#include "runtime/primitive/core.h"
#include "runtime/scene/add.h"
#include "runtime/shader/core.h"
#include "runtime/systems/scene_system.h"

void example_gizmo(Engine *engine) {

  Mesh *gizmo = rem_new_mesh();
  Primitive mbin_primitive;
  loader_mbin_load_primitive(&(MBINLoadPrimitiveDescriptor){
      .path = "./resources/assets/mbin/sphere.mbin",
      .primitive = &mbin_primitive,
  });

  mesh_create_primitive(gizmo, &(MeshCreatePrimitiveDescriptor){
                                   .primitive = &mbin_primitive,
                                   .name = "gizmo",
                               });

  mesh_shader_create(
      gizmo, &(ShaderCreateDescriptor){
                 .pipeline = std_render_pipeline(RenderPipelineType_Line),
                 .name = "gizmo shader",
             });

  mesh_set_position(gizmo, (vec3){2.0f, 3.3f, 2.0f});

  engine_scene_add_mesh_custom(engine, gizmo, NULL,
                               &(RendererBatchKeyDescriptor){
                                   .flags = RendererBatchFlag_Fixed,
                                   .layer = RendererLayer_Gizmo,
                                   .pipeline = RenderPipelineType_Line,
                                   .draw_mode = RendererDrawMode_All,
                               },
                               EngineAddFlag_TreeHide);
}
