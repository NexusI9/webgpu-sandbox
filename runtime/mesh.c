#include "mesh.h"
#include "../backend/generator.h"
#include "shader.h"
#include "webgpu/webgpu.h"
#include <stdio.h>

mesh mesh_create(const MeshCreateDescriptor *md) {

  mesh new_mesh;

  // set wgpu
  new_mesh.wgpu.device = md->wgpu.device;
  new_mesh.wgpu.queue = md->wgpu.queue;

  // set vertices
  new_mesh.vertex.data = md->vertex.data;
  new_mesh.vertex.length = md->vertex.length;

  // set indexes
  new_mesh.index.data = md->index.data;
  new_mesh.index.length = md->index.length;

  // set shader
  new_mesh.shader = md->shader;
  // build shader (set pipeline)
  shader_build(&new_mesh.shader);

  // push vertices & index data to buffer

  // store vertex in buffer
  mesh_create_vertex_buffer(
      &new_mesh,
      &(MeshCreateBufferDescriptor){
          .data = (void *)new_mesh.vertex.data,
          .size = new_mesh.vertex.length * sizeof(new_mesh.vertex.data[0]),
      });

  // store index in buffer
  mesh_create_index_buffer(
      &new_mesh,
      &(MeshCreateBufferDescriptor){
          .data = (void *)new_mesh.index.data,
          .size = new_mesh.index.length * sizeof(new_mesh.vertex.data[0]),
      });

  return new_mesh;
}

mesh mesh_create_primitive(const MeshPrimitiveCreateDescriptor *md) {
  return mesh_create(&(MeshCreateDescriptor){
      .wgpu = md->wgpu,
      .index = md->primitive.index,
      .vertex = md->primitive.vertex,
      .shader = md->shader,
  });
}

// send vertex data to GPU
void mesh_create_vertex_buffer(mesh *mesh,
                               const MeshCreateBufferDescriptor *bd) {

  if (mesh->wgpu.device == NULL || mesh->wgpu.queue == NULL)
    perror("Mesh has no device or queue"), exit(0);

  mesh->buffer.vertex = create_buffer(&(CreateBufferDescriptor){
      .queue = mesh->wgpu.queue,
      .device = mesh->wgpu.device,
      .data = (void *)bd->data,
      .size = bd->size,
      .usage = WGPUBufferUsage_Vertex,
  });
}

// send index data to GPU
void mesh_create_index_buffer(mesh *mesh,
                              const MeshCreateBufferDescriptor *bd) {

  if (mesh->wgpu.device == NULL || mesh->wgpu.queue == NULL)
    perror("Mesh has no device or queue"), exit(0);

  mesh->buffer.index = create_buffer(&(CreateBufferDescriptor){
      .queue = mesh->wgpu.queue,
      .device = mesh->wgpu.device,
      .data = (void *)bd->data,
      .size = bd->size,
      .usage = WGPUBufferUsage_Index,
  });
}

void mesh_draw(mesh *mesh, WGPURenderPassEncoder *render_pass,
               const camera *camera, const viewport *viewport) {

  // draw shader
  shader_draw(&mesh->shader, render_pass, camera, viewport);

  // draw indexes from buffer
  wgpuRenderPassEncoderSetVertexBuffer(*render_pass, 0, mesh->buffer.vertex, 0,
                                       WGPU_WHOLE_SIZE);
  wgpuRenderPassEncoderSetIndexBuffer(*render_pass, mesh->buffer.index,
                                      WGPUIndexFormat_Uint16, 0,
                                      WGPU_WHOLE_SIZE);
  wgpuRenderPassEncoderDrawIndexed(*render_pass, mesh->index.length, 1, 0, 0,
                                   0);
}
