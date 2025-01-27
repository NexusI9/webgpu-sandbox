#include "mesh.h"

mesh mesh_create(const MeshCreateDescriptor *md) {

  mesh new_mesh;

  // vertices
  new_mesh.vertex.data = md->vertex.data;
  new_mesh.vertex.length = md->vertex.length;
  
  // indexes
  new_mesh.index.data = md->index.data;
  new_mesh.index.length = md->index.length;

  return new_mesh;
}
