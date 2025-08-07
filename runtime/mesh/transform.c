#include "transform.h"
#include "../utils/matrix.h"
#include "core.h"
#include "uniform.h"

static void mesh_update_model_matrix(Mesh *);

static void mesh_update_model_uniform(Mesh *);

/**
   Update mesh model matrix on the right order based on the cached position,
   rotation and scale.
   This function is called everytime the mesh follows a spatial transformation.
 */
void mesh_update_model_matrix(Mesh *mesh) {

  mat4 S, R, T, SR;

  glm_mat4_identity(S);
  glm_scale(S, mesh->scale);

  glm_quat_mat4(mesh->rotation_quat, R);

  glm_mat4_identity(T);
  glm_translate(T, mesh->position);

  glm_mat4_mul(R, S, SR);
  glm_mat4_mul(T, SR, mesh->model);

  // update topologies
  mesh_topology_boundbox_update(&mesh->topology.base, mesh->model,
                                &mesh->topology.boundbox, mesh->queue);

  //mesh_update_model_uniform(mesh);
}

static void mesh_update_model_uniform(Mesh *mesh) {

  Shader *shader = mesh->shader.active;

  shader_update_uniform(shader, shader->pipeline->bindings.mvp.group,
                        &mesh->model, sizeof(MeshUniform),
                        shader->pipeline->bindings.mvp.model);
}

/**
   Apply scale to mesh transform matrix
 */
void mesh_scale(Mesh *mesh, vec3 scale) {
  glm_vec3_copy(scale, mesh->scale);

  mesh_update_model_matrix(mesh);
}

void mesh_scale_axis(Mesh *mesh, vec3 value, const Axis axis) {
  vec3 axis_value;
  vec3_replace_axis(mesh->scale, value, axis, &axis_value);

  mesh_scale(mesh, axis_value);
}

/**
   Apply translation to mesh transform matrix
 */
void mesh_translate(Mesh *mesh, vec3 position) {
  glm_vec3_copy(position, mesh->position);

  mesh_update_model_matrix(mesh);
}

void mesh_translate_axis(Mesh *mesh, vec3 value, const Axis axis) {
  vec3 axis_value;
  vec3_replace_axis(mesh->position, value, axis, &axis_value);

  mesh_translate(mesh, axis_value);
}

/**
   Set Euler rotation
 */
void mesh_rotate(Mesh *mesh, vec3 rotation) {
  // cache euler rotation
  glm_vec3_copy(rotation, mesh->rotation_euler);

  // update quat from euler
  vec3 rad_rotation;
  glm_vec3_scale(mesh->rotation_euler, GLM_PI / 180.0f, rad_rotation);

  glm_euler_xyz_quat(rad_rotation, mesh->rotation_quat);

  // recompute model matrix
  mesh_update_model_matrix(mesh);
}

void mesh_rotate_axis(Mesh *mesh, vec3 value, const Axis axis) {
  vec3 axis_value;
  vec3_replace_axis(mesh->rotation_euler, value, axis, &axis_value);

  mesh_rotate(mesh, axis_value);
}

/**
   Apply rotation to mesh transform matrix
 */
void mesh_rotate_quat(Mesh *mesh, versor rotation) {

  // cache quat rotation
  glm_quat_copy(rotation, mesh->rotation_quat);

  // update mesh euler rotation
  mat4 rot;
  glm_quat_mat4(mesh->rotation_quat, rot);

  vec3 rad_euler;
  glm_euler_angles(rot, rad_euler);
  glm_vec3_scale(rad_euler, 180.0f / GLM_PI, mesh->rotation_euler);

  // recompute model matrix
  mesh_update_model_matrix(mesh);
}

/**
   Apply look at transformation to mesh
 */
void mesh_lookat(Mesh *mesh, vec3 position, vec3 target) {

  glm_mat4_identity(mesh->model);

  matrix_mesh_lookat(&(UtilsMatrixLookatDescriptor){
      .dest_position = &mesh->position,
      .dest_matrix = &mesh->model,
      .position = position,
      .target = target,
      .up = NULL,
      .forward = NULL,
      .right = NULL,
  });
}
