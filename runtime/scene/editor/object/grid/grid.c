#include "grid.h"
#include "../runtime/material/material.h"
#include "../runtime/primitive/plane.h"
#include "webgpu/webgpu.h"

void seo_grid_create(Mesh *mesh, GizmoGridCreateDescriptor *gd) {

  Primitive plane = primitive_plane();

  mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                  .name = "grid",
                                  .queue = gd->queue,
                                  .device = gd->device,
                                  .primitive = plane,
                              });

  mesh_set_shader(mesh, &(ShaderCreateDescriptor){
                            .pipeline = gd->pipeline,
                            .label = "grid",
                            .name = "grid",
                            .device = gd->device,
                            .queue = gd->queue,
                        });

  mesh_scale(mesh, (vec3){
                       gd->uniform.size,
                       gd->uniform.size,
                       gd->uniform.size,
                   });

  material_texture_add_uniform(
      mesh, &(ShaderCreateUniformDescriptor){
                .group_index = 1,
                .entry_count = 1,
                .entries =
                    (ShaderBindGroupUniformEntry[]){
                        {
                            .binding = 0,
                            .data = &gd->uniform,
                            .size = sizeof(GizmoGridUniform),
                            .offset = 0,
                        },
                    },
                .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
            });
}
