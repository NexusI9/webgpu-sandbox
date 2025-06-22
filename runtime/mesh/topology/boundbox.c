#include "boundbox.h"
#include "../backend/buffer.h"
#include "core.h"

static void mesh_topology_boundbox_cube(MeshTopologyBoundbox *);

/**
   Boundbox topology has a double process:

   1. Data: the bound attributes, that will be used for physic and raycast (CPU
   computing)

   2. Vis Rep: the casual topology vertex and index attributes used for visual
   representation (GPU)

       y               The current boundbox topology uses a Axis-Aligned
       |               Bound box (AABB). Meaning it's visual representtion
       |               is a simple cube.
       .------------.
      /|           /|
     / |          / |
    '--+---------'  |
    |  |         |  |
    |  '---------|--'---------x
    | /          | /
    |/           |/
    '------------'

    For optimisation purpose, the two processes are divided into two separate
   functions:

   1. mesh_topology_boundbox_compute_bound(): only takes care of calculating
   the AABB (physics/collisition data)

   2. mesh_topology_boundbox_create(): takes care of the bound calculation AND
   visual representation (cube wireframe)


   The compute bound function (1) will be called in the default render mode
   (texture/ wireframe...)
   Since in these mode we actually only care about the bound data itself (for
   the raycast) rather than its (invisible) visual representation.

   However in the Boundbox render more, we will take take to actually render the
   wireframe cube since it will be drawn (2).
*/

/**
   This function simply calculate the AABB according to the base topology
   without handling the topology visual representation.

 */
void mesh_topology_boundbox_compute_bound(MeshTopologyBase *base,
                                          MeshTopologyBoundbox *bound) {

  VertexAttribute *base_attr = &base->attribute;
  glm_vec3_copy((vec3){0}, bound->bound.min);
  glm_vec3_copy((vec3){0}, bound->bound.max);

  for (size_t i = 0; i < base->attribute.length; i += VERTEX_STRIDE) {
    vattr_t *current = &base_attr->entries[i];
    glm_vec3_minv(bound->bound.min, current, bound->bound.min);
    glm_vec3_maxv(bound->bound.max, current, bound->bound.max);
  }
}

int mesh_topology_boundbox_create(MeshTopologyBase *base,
                                  MeshTopologyBoundbox *bound,
                                  const WGPUDevice *device,
                                  const WGPUQueue *queue) {

  // allocate vertex + index attribute
  // 12 edges * 2 vertex (/edges)
  size_t edge_count = 12;
  size_t attr_len = edge_count * 2 * VERTEX_STRIDE;
  bound->attribute.entries = calloc(attr_len, sizeof(vattr_t));
  bound->attribute.capacity = attr_len;
  bound->attribute.length = 0;

  // 12 edges * 6 indices per edges
  size_t index_len = edge_count * 6;
  bound->index.entries = calloc(index_len, sizeof(vindex_t));
  bound->index.capacity = index_len;
  bound->index.length = 0;

  // compute bounds
  mesh_topology_boundbox_compute_bound(base, bound);

  // create cube based on bounds
  mesh_topology_boundbox_cube(bound);

  // upload to gpu
  buffer_create(&bound->index.buffer,
                &(CreateBufferDescriptor){
                    .queue = queue,
                    .device = device,
                    .data = (void *)bound->index.entries,
                    .size = bound->index.length * sizeof(vindex_t),
                    .usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst,
                    .mappedAtCreation = false,
                });

  buffer_create(&bound->attribute.buffer,
                &(CreateBufferDescriptor){
                    .queue = queue,
                    .device = device,
                    .data = (void *)bound->attribute.entries,
                    .size = bound->attribute.length * sizeof(vattr_t),
                    .usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst,
                    .mappedAtCreation = false,
                });

  return MESH_TOPOLOGY_BOUNDBOX_SUCCESS;
}

void mesh_topology_boundbox_cube(MeshTopologyBoundbox *bound) {}

MeshTopology mesh_topology_boundbox_vertex(MeshTopologyBoundbox *bound) {
  return (MeshTopology){
      .attribute = &bound->attribute,
      .index = &bound->index,
  };
}

int mesh_topology_boundbox_update(const MeshTopologyBase *base,
                                  MeshTopologyBoundbox *bound,
                                  const WGPUQueue *queue) {

  // get base min max

  // update vertex attribute

  // update cube

  // update buffer
  wgpuQueueWriteBuffer(*queue, bound->attribute.buffer, 0,
                       bound->attribute.entries,
                       bound->attribute.length * sizeof(vattr_t));

  return MESH_TOPOLOGY_BOUNDBOX_SUCCESS;
}
