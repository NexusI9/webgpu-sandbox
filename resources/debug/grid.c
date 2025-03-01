#include "grid.h"
#include "../primitive/plane.h"

GridUniform grid_uniform(const grid *grid) {
  return (GridUniform){.cell_size = grid->cell_size, .size = grid->size};
}
mesh grid_create_mesh(const GridCreateDescriptor *gd) {

  grid new_grid;
  new_grid.cell_size = gd->cell_size;
  new_grid.size = gd->size;

  primitive plane = primitive_plane();

  shader shader = shader_create(&(ShaderCreateDescriptor){
      .path = "./shader/grid.wgsl",
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

  mesh_scale(&grid_mesh, (vec3){new_grid.size, new_grid.size, new_grid.size});

  // bind camera and viewport
  // NOTE: binding groups shall be created in order (0 first, then 1)

  mesh_bind_matrices(&grid_mesh, gd->camera, gd->viewport, 0);

  GridUniform uGrid = grid_uniform(&new_grid);

  ShaderBindGroupEntry grid_entries[1] = {
      {
          .binding = 0,
          .data = &uGrid,
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
