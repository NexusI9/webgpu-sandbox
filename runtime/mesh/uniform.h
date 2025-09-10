#ifndef _MESH_UNIFORM_H_
#define _MESH_UNIFORM_H_

#include "core.h"

MeshUniform *mesh_uniform(Mesh *);

void mesh_uniform_update(Mesh *);

void mesh_uniform_model_update_callback(void *, void *);
bool mesh_uniform_model_compare_callback(void *, const void *);

static inline void mesh_uniform_set_probe_reflection_plane(Mesh *mesh,
                                                           const ssbo_id_t id,
                                                           SSBOManager *ssbo) {
  MeshUniform *uniform = mesh_uniform(mesh);

  if (uniform->probe_reflection_plane_id == id &&
      uniform->probe_reflection_plane_count == 1)
    return;

  uniform->probe_reflection_plane_count = 1;
  uniform->probe_reflection_plane_id = id;

  ssbo_update_queue_insert(ssbo, SSBOType_Mesh, mesh->ssbo_slot.id);
}

static inline void
mesh_uniform_clear_probe_reflection_plane(Mesh *mesh, SSBOManager *ssbo) {

  MeshUniform *uniform = mesh_uniform(mesh);

  if (uniform->probe_reflection_plane_count == 0)
    return;

  uniform->probe_reflection_plane_count = 0;

  ssbo_update_queue_insert(ssbo, SSBOType_Mesh, mesh->ssbo_slot.id);
}

#endif
