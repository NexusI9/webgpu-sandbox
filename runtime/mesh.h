#ifndef _MESH_H_
#define _MESH_H_
#include <stdint.h>

//Builder Pattern | Descriptor Pattern
typedef struct{
    
    const float* vertex;
    const uint16_t vertex_length;
    
    const uint16_t* index;
    const uint16_t index_length;
    
} MeshDescriptor;

typedef struct{

    uint8_t id;
    
    const float* vertex;
    uint16_t vertex_length;
    
    const uint16_t* index;
    uint16_t index_length;
    
} mesh;

mesh mesh_create(const MeshDescriptor*);

#endif
