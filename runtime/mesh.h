#ifndef _MESH_H_
#define _MESH_H_
#include <stdint.h>

typedef struct{

    const uint8_t id;
    
    const float* vertex;
    const uint16_t vertex_length;
    
    const uint16_t* index;
    const uint16_t index_length;
    
    
} mesh;

#endif
