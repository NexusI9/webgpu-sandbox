#include "icosphere.h"

#include "runtime/geometry/vertex/attribute.h"
#include "runtime/geometry/vertex/index.h"

// golden ratio constants
static const float X = 0.525731112119133606f;
static const float Z = 0.850650808352039932f;

static vattr_t icosphere_vertex_data[] = {
    // pos (x,y,z),       normal (nx,ny,nz),       tangent (tx,ty,tz,w),    color (r,g,b),       uv (u,v)

    -X, 0.0f,  Z,    X, 0.0f, -Z,    0.0f, 1.0f, 0.0f, 1.0f,    1.0f,0.0f,0.0f,   0.0f,0.0f, 
     X, 0.0f,  Z,   -X, 0.0f, -Z,    0.0f, 1.0f, 0.0f, 1.0f,    0.0f,1.0f,0.0f,   1.0f,0.0f, 
    -X, 0.0f, -Z,    X, 0.0f,  Z,    0.0f, 1.0f, 0.0f, 1.0f,    0.0f,0.0f,1.0f,   0.0f,1.0f, 
     X, 0.0f, -Z,   -X, 0.0f,  Z,    0.0f, 1.0f, 0.0f, 1.0f,    1.0f,1.0f,0.0f,   1.0f,1.0f, 
     0.0f,  Z,  X,   0.0f, -Z, -X,   1.0f, 0.0f, 0.0f, 1.0f,    0.0f,1.0f,1.0f,   0.5f,0.0f, 
     0.0f,  Z, -X,   0.0f, -Z,  X,   1.0f, 0.0f, 0.0f, 1.0f,    1.0f,0.0f,1.0f,   0.5f,1.0f, 
     0.0f, -Z,  X,   0.0f,  Z, -X,   1.0f, 0.0f, 0.0f, 1.0f,    1.0f,0.5f,0.0f,   0.5f,0.5f, 
     0.0f, -Z, -X,   0.0f,  Z,  X,   1.0f, 0.0f, 0.0f, 1.0f,    0.5f,1.0f,0.5f,   0.5f,0.5f, 
     Z,  X, 0.0f,   -Z,-X, 0.0f,    0.0f,0.0f,1.0f, 1.0f,       0.5f,0.0f,1.0f,   1.0f,0.5f, 
    -Z,  X, 0.0f,    Z,-X, 0.0f,    0.0f,0.0f,1.0f, 1.0f,       1.0f,0.5f,1.0f,   0.0f,0.5f, 
     Z, -X, 0.0f,   -Z, X, 0.0f,    0.0f,0.0f,1.0f, 1.0f,       0.0f,1.0f,0.5f,   1.0f,0.5f, 
    -Z, -X, 0.0f,    Z, X, 0.0f,    0.0f,0.0f,1.0f, 1.0f,       0.5f,0.5f,0.5f,   0.0f,0.5f, 
};

static vindex_t icosphere_index_data[] = {
    0, 1, 4,   0, 4, 9,   9, 4, 5,   4, 8, 5,   4, 1, 8,
    8, 1,10,   8,10, 3,   5, 8, 3,   5, 3, 2,   2, 3, 7,
    7, 3,10,   7,10, 6,   7, 6,11,  11, 6, 0,   0, 6, 1,
    6,10, 1,   9,11, 0,   9, 2,11,   9, 5, 2,   7,11, 2
};

Primitive primitive_icosphere() {
    return (Primitive){
        .vertex =
            {
                .entries = icosphere_vertex_data,
                .count = sizeof(icosphere_vertex_data) / sizeof(icosphere_vertex_data[0]),
                .capacity = sizeof(icosphere_vertex_data) / sizeof(icosphere_vertex_data[0])
            },

        .index =
            {
                .entries = icosphere_index_data,
                .count = sizeof(icosphere_index_data) / sizeof(icosphere_index_data[0]),
                .capacity = sizeof(icosphere_index_data) / sizeof(icosphere_index_data[0])
            },
    };
}
