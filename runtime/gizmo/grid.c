#include "grid.h"
#include "../resources/primitive/plane.h"
#include "../runtime/material/material.h"
#include "webgpu/webgpu.h"

void gizmo_grid_create(Mesh *mesh, GizmoGridCreateDescriptor *gd) {

  Primitive plane = primitive_plane();

  mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                  .name = "grid",
                                  .queue = gd->queue,
                                  .device = gd->device,
                                  .primitive = plane,
                              });

  mesh_set_shader(mesh, &(ShaderCreateDescriptor){
                            .path = "./runtime/assets/shader/shader.grid.wgsl",
                            .label = "grid",
                            .name = "grid",
                            .device = gd->device,
                            .queue = gd->queue,
                        });

  pipeline_set_primitive(&mesh_shader_texture(mesh)->pipeline,
                         (WGPUPrimitiveState){
                             .frontFace = WGPUFrontFace_CCW,
                             .cullMode = WGPUCullMode_None,
                             .topology = WGPUPrimitiveTopology_TriangleList,
                             .stripIndexFormat = WGPUIndexFormat_Undefined,
                         });
  material_texture_double_sided(mesh);

  mesh_scale(mesh, (vec3){
                       gd->uniform.size,
                       gd->uniform.size,
                       gd->uniform.size,
                   });

  // bind camera and viewport
  // NOTE: binding groups shall be created in order (0 first, then 1)
  material_texture_bind_views(mesh, gd->camera, gd->viewport, 0);

  material_texture_add_uniform(
      mesh, &(ShaderCreateUniformDescriptor){
                .group_index = 1,
                .entry_count = 1,
                .entries =
                    (ShaderBindGroupUniformEntry[]){
                        {
                            .binding = 0,
                            .data = &gd->uniform,
                            .size = sizeof(GridUniform),
                            .offset = 0,
                        },
                    },
                .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
            });
}
