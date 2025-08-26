#include "grid.h"
#include "../backend/renderer/scene/std_pipeline/std_pipeline.h"
#include "../runtime/mesh/shader/shader.h"
#include "../runtime/primitive/plane.h"
#include "webgpu/webgpu.h"

void seo_grid_create(Mesh *mesh, GizmoGridCreateDescriptor *gd) {

  Primitive plane = primitive_plane();

  mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                  .name = "grid",
                                  .queue = gd->queue,
                                  .device = gd->device,
                                  .primitive = &plane,
                              });

  mesh_shader_create_fixed(mesh,
                           &(ShaderCreateDescriptor){
                               .pipeline = std_pipeline(PipelineType_Grid),
                               .label = "grid",
                               .name = "grid",
                               .device = gd->device,
                               .queue = gd->queue,
                           });

  mesh_set_scale(mesh, (vec3){
                       gd->uniform.size,
                       gd->uniform.size,
                       gd->uniform.size,
                   });

  shader_update_uniform_data(mesh_shader(mesh, MeshShader_Fixed), 1, 0, &gd->uniform);

}
