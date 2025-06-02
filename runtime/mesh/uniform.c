#include "uniform.h"
#include "../../utils/system.h"
#include <string.h>

MeshUniform mesh_uniform_model(Mesh *mesh) {

  MeshUniform uModel;

  vec4 position = {
      mesh->position[0],
      mesh->position[1],
      mesh->position[2],
      1.0f,
  };

  glm_mat4_copy(mesh->model, uModel.model);
  glm_vec4_copy(position, uModel.position);

  return uModel;
}

void mesh_uniform_model_update(void *callback_mesh, void *entry_data) {

  Mesh *cast_mesh = (Mesh *)callback_mesh;
  MeshUniform *new_data = (MeshUniform *)entry_data;

  //  transfer updated camera values (position and view) to new data
  MeshUniform uMesh = mesh_uniform_model(cast_mesh);
  glm_mat4_copy(uMesh.model, new_data->model);
  glm_vec4_copy(uMesh.position, new_data->position);
}

bool mesh_uniform_model_compare(void *callback_data, const void *entry_data) {

  Mesh *cast_mesh = (Mesh *)callback_data;
  MeshUniform *cast_uniform = (MeshUniform *)entry_data;

  return memcmp(cast_mesh->model, cast_uniform->model, sizeof(mat4)) != 0;
}
