#include "uniform.h"
#include "../../utils/system.h"
#include "core.h"
#include <string.h>

MeshUniform *mesh_uniform(Mesh *mesh) { return mesh->uniform; }

void mesh_uniform_update(Mesh *mesh) {

  vec4 position = {
      mesh->position[0],
      mesh->position[1],
      mesh->position[2],
      1.0f,
  };

  glm_mat4_copy(mesh->model, mesh->uniform->model);
  glm_vec4_copy(position, mesh->uniform->position);
}

void mesh_uniform_model_update_callback(void *callback_mesh, void *entry_data) {

  Mesh *cast_mesh = (Mesh *)callback_mesh;
  MeshUniform *new_data = (MeshUniform *)entry_data;

  //  transfer updated camera values (position and view) to new data
  glm_mat4_copy(cast_mesh->uniform->model, new_data->model);
  glm_vec4_copy(cast_mesh->uniform->position, new_data->position);
}

bool mesh_uniform_model_compare_callback(void *callback_data,
                                         const void *entry_data) {

  Mesh *cast_mesh = (Mesh *)callback_data;
  MeshUniform *cast_uniform = (MeshUniform *)entry_data;

  return memcmp(cast_mesh->model, cast_uniform->model, sizeof(mat4)) != 0;
}
