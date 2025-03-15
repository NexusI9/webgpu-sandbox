#include "grid.h"
#include "../primitive/plane.h"

void grid_create_mesh(mesh *mesh, GridCreateDescriptor *gd) {

  primitive plane = primitive_plane();
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

  mesh_scale(mesh, (vec3){
                       gd->uniform.size,
                       gd->uniform.size,
                       gd->uniform.size,
                   });
  // bind camera and viewport
  // NOTE: binding groups shall be created in order (0 first, then 1)

  mesh_bind_matrices(mesh, gd->camera, gd->viewport, 0);

  ShaderBindGroupEntry grid_entries[1] = {
      {
          .binding = 0,
          .data = &gd->uniform,
          .size = sizeof(GridUniform),
          .offset = 0,
      },
  };

  shader_add_uniform(&mesh->shader, &(ShaderCreateUniformDescriptor){
                                        .group_index = 1,
                                        .entry_count = 1,
                                        .entries = grid_entries,
                                        .visibility = WGPUShaderStage_Vertex |
                                                      WGPUShaderStage_Fragment,
                                    });
}
