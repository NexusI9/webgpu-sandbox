#include "solid.h"

void material_solid_bind_views(Mesh *mesh, Camera *camera, Viewport *viewport,
                               uint8_t group_index) {
  material_bind_views(mesh, mesh_shader_solid, camera, viewport, group_index);
}


/**
  update pipeline for double-sided
 */
void material_solid_double_sided(Mesh *mesh) {
  pipeline_set_primitive(shader_pipeline(mesh_shader_solid(mesh)),
                         (WGPUPrimitiveState){
                             .frontFace = WGPUFrontFace_CCW,
                             .cullMode = WGPUCullMode_None,
                             .topology = WGPUPrimitiveTopology_TriangleList,
                             .stripIndexFormat = WGPUIndexFormat_Undefined,
                         });
}
