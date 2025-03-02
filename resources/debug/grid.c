#include "grid.h"
#include "../primitive/plane.h"

mesh grid_create_mesh(GridCreateDescriptor *gd) {

  primitive plane = primitive_plane();

  shader shader = shader_create(&(ShaderCreateDescriptor){
      .path = "./runtime/assets/shader/grid.wgsl",
      .label = "grid",
      .name = "grid",
      .device = gd->device,
      .queue = gd->queue,
  });

  mesh grid_mesh = mesh_create_primitive(&(MeshCreatePrimitiveDescriptor){
      .wgpu =
          {
              .queue = gd->queue,
              .device = gd->device,
          },
      .primitive = plane,
      .shader = shader,
  });

  mesh_scale(&grid_mesh, (vec3){
                             gd->uniform.size,
                             gd->uniform.size,
                             gd->uniform.size,
                         });

  // bind camera and viewport
  // NOTE: binding groups shall be created in order (0 first, then 1)

  mesh_bind_matrices(&grid_mesh, gd->camera, gd->viewport, 0);

  ShaderBindGroupEntry grid_entries[1] = {
      {
          .binding = 0,
          .data = &gd->uniform,
          .size = sizeof(GridUniform),
          .offset = 0,
      },
  };

  shader_add_uniform(
      &grid_mesh.shader,
      &(ShaderCreateUniformDescriptor){
          .group_index = 1,
          .entry_count = 1,
          .entries = grid_entries,
          .visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment,
      });

  return grid_mesh;
}
