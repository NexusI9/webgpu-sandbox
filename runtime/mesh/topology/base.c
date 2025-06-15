#include "base.h"
#include "../backend/buffer.h"
#include "../utils/system.h"
#include "anchor.h"
#include <string.h>

static void mesh_topology_base_create_anchor(MeshTopologyBase *);

/**
   Handle the base topology creation as well as anchor generation
 */
void mesh_topology_base_create(MeshTopologyBase *base,
                               const VertexAttribute *va, const VertexIndex *vi,
                               const WGPUDevice *device,
                               const WGPUQueue *queue) {

  // create vertex attributes
  mesh_topology_base_create_vertex_attribute(base, va, device, queue);

  // create index attributes
  mesh_topology_base_create_vertex_index(base, vi, device, queue);

  // create anchors
  mesh_topology_base_create_anchor(base);
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
   Cache siblings anchor for each vertex.
 */
void mesh_topology_base_create_anchor(MeshTopologyBase *base) {

  printf("create base anchor\n");
  MeshTopologyAnchorList hashed_list; // temp

  // init new list
  mesh_topology_anchor_list_create(&hashed_list,
                                   MESH_TOPOLOGY_ANCHOR_LIST_DEFAULT_CAPACITY);

  // 1. store based on position (hash)
  for (size_t i = 0; i < base->index.length; i++) {

    vindex_t base_index = base->index.entries[i];
    vattr_t *base_vertex = &base->attribute.entries[base_index * VERTEX_STRIDE];
    vec3 position;
    memcpy(&position, base_vertex, sizeof(vertex_position));

    mesh_topology_anchor_list_insert(&hashed_list, &position, &base_index, 1);
  }

  // 2. remmap based on index (linear)
  MeshTopologyAnchorList *mapped_list = &base->siblings;

  // destroy if already exists
  if (mapped_list->entries != NULL)
    mesh_topology_anchor_list_destroy(mapped_list);

  // create mapped list
  mesh_topology_anchor_list_create(mapped_list,
                                   MESH_TOPOLOGY_ANCHOR_LIST_DEFAULT_CAPACITY);
  // remap
  MeshTopology base_topo = mesh_topology_base_vertex(base);
  mesh_topology_anchor_list_map(&hashed_list, &base_topo, mapped_list);
}

void mesh_topology_base_scale(MeshTopologyBase *base,
                              const VertexIndexSelection *select, vec3 *scale) {

  // combine all anchors
  MeshTopologyAnchor combined_anchor;
  mesh_topology_anchor_create(&combined_anchor,
                              MESH_TOPOLOGY_ANCHOR_DEFAULT_CAPACITY);

  MeshTopologyAnchorList *anchors = &base->siblings;

  for (size_t i = 0; i < select->length; i++) {
    vindex_t index = select->entries[i];
    MeshTopologyAnchor *index_anchor = &anchors->entries[index];
    mesh_topology_anchor_merge(anchors, index_anchor->entries,
                               index_anchor->length, &combined_anchor);
  }

  // apply transform with all combined anchors
  vertex_transform_scale(
      &(VertexIndexSelection){
          .entries = combined_anchor.entries,
          .length = combined_anchor.length,
      },
      &base->attribute, scale);
}
