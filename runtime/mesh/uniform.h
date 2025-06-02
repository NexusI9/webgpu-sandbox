#ifndef _MESH_UNIFORM_H_
#define _MESH_UNIFORM_H_

#include "core.h"

typedef struct {
  mat4 model;
  vec4 position;
} __attribute__((aligned(16))) MeshUniform;

MeshUniform mesh_uniform_model(Mesh *);

void mesh_uniform_model_update(void*, void*);
bool mesh_uniform_model_compare(void*, const void*);

#endif
