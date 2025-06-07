#include "wireframe.h"
#include "../material/material.h"
#include "webgpu/webgpu.h"

/**
   Setup a wireframe mesh with the given vertex/index attributes and color
 */
void gizmo_create_wireframe(Mesh *mesh,
                            const GizmoCreateWireframeDescriptor *desc) {

  // set wireframe color from the vertex attributes
  vertex_attribute_set_color(desc->vertex, desc->color);

  // create mesh from vertex/index attributes
  mesh_create(mesh, &(MeshCreateDescriptor){
                        .device = desc->device,
                        .queue = desc->queue,
                        .index = *desc->index,
                        .vertex = *desc->vertex,
                        .name = desc->name,
                    });

  // set wireframe shader
  mesh_set_shader(mesh, &(ShaderCreateDescriptor){
                            .device = desc->device,
                            .queue = desc->queue,
                            .label = "Gizmo wireframe shader",
                            .name = "Gizmo wireframe shader",
                            .path = SHADER_PATH_LINE,
                        });

  // add wireframe thickness
  float thickness = GIZMO_WIREFRAME_LINE_THICKNESS;
  shader_add_uniform(
      mesh_shader_texture(mesh),
      &(ShaderCreateUniformDescriptor){
          .entry_count = 1,
          .group_index = 1,
          .visibility = WGPUShaderStage_Fragment | WGPUShaderStage_Vertex,
          .entries = (ShaderBindGroupUniformEntry[]){{
              .binding = 0,
              .size = sizeof(float),
              .data = &thickness,
              .offset = 0,
          }},
      });

  // set double sided
  material_texture_double_sided(mesh);
}
