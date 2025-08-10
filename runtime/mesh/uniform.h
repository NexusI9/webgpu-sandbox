#ifndef _MESH_UNIFORM_H_
#define _MESH_UNIFORM_H_

#include "core.h"

MeshUniform *mesh_uniform(Mesh *);

void mesh_uniform_update(Mesh *);

void mesh_uniform_model_update_callback(void *, void *);
bool mesh_uniform_model_compare_callback(void *, const void *);

#endif
