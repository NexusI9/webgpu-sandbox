#include "base.h"
#include "../backend/buffer.h"

static void mesh_topology_base_create_anchor(MeshTopologyBase *);

/**
   Handle the base topology creation as well as anchor generation
 */
void mesh_topology_base_create(MeshTopologyBase *base, VertexAttribute *va,
                               VertexIndex *vi, const WGPUDevice *device,
                               const WGPUQueue *queue) {

  // create vertex attributes
  mesh_topology_base_create_vertex_attribute(base, va, device, queue);

  // create index attributes
  mesh_topology_base_create_vertex_index(base, vi, device, queue);

  // create anchors
}

/**
   Return the MeshTopology of base vertex (without anchor)
 */
MeshTopology mesh_topology_base_vertex(MeshTopologyBase *topo) {
  return (MeshTopology){
      .attribute = &topo->attribute,
      .index = &topo->index,
  };
}

/**
   Create the base vertex attributes and upload data to buffer
 */
int mesh_topology_base_create_vertex_attribute(MeshTopologyBase *base,
                                               const VertexAttribute *va,
                                               const WGPUDevice *device,
                                               const WGPUQueue *queue) {

  // reset buffer
  if (base->attribute.buffer) {
    wgpuBufferRelease(base->attribute.buffer);
    base->attribute.buffer = NULL;
  }

  base->attribute.entries = va->entries;
  base->attribute.length = va->length;
  base->attribute.capacity = va->length;

  if (base->attribute.length) {
    if (device == NULL || queue == NULL)
      perror("Mesh has no device or queue "), exit(0);

    buffer_create(&base->attribute.buffer,
                  &(CreateBufferDescriptor){
                      .queue = queue,
                      .device = device,
                      .data = (void *)base->attribute.entries,
                      .size = base->attribute.length * sizeof(vindex_t),
                      .usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst,
                      .mappedAtCreation = false,
                  });
    return MESH_TOPOLOGY_BASE_SUCCESS;
  }

  return MESH_TOPOLOGY_BASE_EMPTY;
}

/**
   Create the base index attributes and upload data to buffer
 */
int mesh_topology_base_create_vertex_index(MeshTopologyBase *base,
                                              const VertexIndex *vi,
                                              const WGPUDevice *device,
                                              const WGPUQueue *queue) {

  // reset buffer
  if (base->index.buffer) {
    wgpuBufferRelease(base->index.buffer);
    base->index.buffer = NULL;
  }

  base->index.entries = vi->entries;
  base->index.length = vi->length;
  base->index.capacity = vi->length;

  if (base->index.length) {

    if (device == NULL || queue == NULL)
      perror("Mesh has no device or queue"), exit(0);

    buffer_create(&base->index.buffer,
                  &(CreateBufferDescriptor){
                      .queue = queue,
                      .device = device,
                      .data = (void *)base->index.entries,
                      .size = base->index.length * sizeof(vindex_t),
                      .usage = WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst,
                      .mappedAtCreation = false,
                  });

    return MESH_TOPOLOGY_BASE_SUCCESS;
  }

  return MESH_TOPOLOGY_BASE_EMPTY;
}

/**
   Cache siblings anchor for each vertex
 */
void mesh_topology_base_create_anchor(MeshTopologyBase *base) {}
