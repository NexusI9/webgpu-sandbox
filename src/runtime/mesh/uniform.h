#ifndef _MESH_UNIFORM_H_
#define _MESH_UNIFORM_H_

#include <stdbool.h>

#include "core.h"
#include "backend/ubo.h"

MeshUniform *mesh_uniform(Mesh *);

void mesh_uniform_update(Mesh *);

void mesh_uniform_model_update_callback(void *, void *);
bool mesh_uniform_model_compare_callback(void *, const void *);

static inline void mesh_uniform_set_probe_reflection_plane(Mesh *mesh,
                                                           UBOManager *ubo) {
  MeshUniform *uniform = mesh_uniform(mesh);

  if (uniform->probe_reflection_plane_count == 1)
    return;

  uniform->probe_reflection_plane_count = 1;

  ubo_update_queue_insert(ubo, UBOType_Mesh, mesh->ubo_slot.id);
}

static inline void
mesh_uniform_clear_probe_reflection_plane(Mesh *mesh, UBOManager *ubo) {

  MeshUniform *uniform = mesh_uniform(mesh);

  if (uniform->probe_reflection_plane_count == 0)
    return;

  uniform->probe_reflection_plane_count = 0;

  ubo_update_queue_insert(ubo, UBOType_Mesh, mesh->ubo_slot.id);
}

#endif
