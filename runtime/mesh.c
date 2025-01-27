#include "mesh.h"

mesh mesh_create(const MeshDescriptor* md){

    mesh new_mesh;

    //vertices
    new_mesh.vertex = md->vertex;
    new_mesh.vertex_length = md->vertex_length;

    //indexes
    new_mesh.index = md->index;
    new_mesh.index_length = md->index_length;
   
    return new_mesh;
    
}
