#include "boundbox.h"

#include <stdbool.h>
#include <stdlib.h>

#include "backend/buffer.h"
#include "core.h"
#include "webgpu/webgpu.h"
#include "runtime/geometry/line/core.h"
#include "runtime/geometry/aabb/aabb.h"
#include "runtime/geometry/vertex/core.h"

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
void mesh_topology_boundbox_compute_bound(const MeshTopologyBase *base,
                                          mat4 model_matrix,
                                          MeshTopologyBoundbox *bound) {

  // calculate local space bound
  aabb_from_vert_attr(&bound->local, &base->attribute);
  aabb_copy(&bound->local, &bound->world);

  // compute corners
  aabb_corners(&bound->local, bound->corners);

  // transform bound to world space
  aabb_to_worldspace(&bound->world, bound->corners, model_matrix);
}

MeshTopologyBoundboxStatus
mesh_topology_boundbox_create(MeshTopologyBase *base, mat4 model_matrix,
                              MeshTopologyBoundbox *bound,
                              const WGPUDevice device, const WGPUQueue queue) {

  // allocate vertex + index attribute
  // 12 edges * 4 vertex (/edges)
  size_t edge_count = 12;
  size_t attr_len = edge_count * 4 * VERTEX_STRIDE;
  bound->attribute = (VertexAttribute){
      .entries = calloc(attr_len, sizeof(vattr_t)),
      .capacity = attr_len,
      .length = 0,
      .buffer = NULL,
  };

  // 12 edges * 6 indices per edges
  size_t index_len = 2 * edge_count * 6;
  bound->index = (VertexIndex){
      .entries = calloc(index_len, sizeof(vindex_t)),
      .capacity = index_len,
      .length = 0,
      .buffer = NULL,
  };

  // compute bounds
  mesh_topology_boundbox_compute_bound(base, model_matrix, bound);

  // create cube based on bounds
  mesh_topology_boundbox_cube(bound);

  // upload to gpu
  buffer_create(&bound->index.buffer,
                &(CreateBufferDescriptor){
                    .queue = queue,
                    .device = device,
                    .data = (void *)bound->index.entries,
                    .size = bound->index.length * sizeof(vindex_t),
                    .usage = WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst,
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

  return MeshTopologyBoundboxStatus_Success;
}

/**
   Create a cube edge based on bound corners.

        6---------7
       /|        /|
      / |       / |
     2--+------3  |
     |  |      |  |
     |  4------+--5
     | /       | /
     0---------1'

 */
void mesh_topology_boundbox_cube(MeshTopologyBoundbox *bound) {

  vec3 *corners = bound->corners;
  ivec2 edges[12] = {
      {0, 1}, {1, 3}, {3, 2}, {2, 0}, {6, 2}, {3, 7},
      {0, 4}, {1, 5}, {4, 5}, {5, 7}, {7, 6}, {6, 4},
  };

  for (size_t i = 0; i < 12; i++)
    line_add_point(bound->corners[edges[i][0]], bound->corners[edges[i][1]],
                   (vec3){1.0f, 1.0f, 1.0f}, &bound->attribute, &bound->index);
}

MeshTopology mesh_topology_boundbox_vertex(MeshTopologyBoundbox *bound) {
  return (MeshTopology){
      .attribute = &bound->attribute,
      .index = &bound->index,
  };
}

MeshTopologyBoundboxStatus
mesh_topology_boundbox_update(const MeshTopologyBase *base, mat4 model,
                              MeshTopologyBoundbox *bound,
                              const WGPUQueue queue) {

  // get base min max
  mesh_topology_boundbox_compute_bound(base, model, bound);

  // update vertex attribute

  // update cube

  // update buffer
  /*wgpuQueueWriteBuffer(queue, bound->attribute.buffer, 0,
                       bound->attribute.entries,
                       bound->attribute.length * sizeof(vattr_t));*/

  return MeshTopologyBoundboxStatus_Success;
}
