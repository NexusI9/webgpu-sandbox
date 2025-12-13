#include "transform.h"

#include <cglm/affine-pre.h>
#include <cglm/affine.h>
#include <cglm/euler.h>
#include <cglm/mat4.h>
#include <cglm/quat.h>
#include <cglm/vec3.h>
#include <stddef.h>

#include "core.h"
#include "topology/boundbox.h"
#include "uniform.h"
#include "utils/matrix.h"
#include "utils/vector/core.h"

static void mesh_update_model_matrix(Mesh *);

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
                                &mesh->topology.boundbox);
}

/**
   Apply scale to mesh transform matrix
 */
void mesh_set_scale(Mesh *mesh, const vec3 scale) {
  glm_vec3_copy((float *)scale, mesh->scale);

  mesh_update_model_matrix(mesh);

  // update children
  for (size_t i = 0; i < mesh->children.count; i++)
    mesh_set_scale(mesh->children.entries[i], scale);
}

void mesh_set_scale_axis(Mesh *mesh, const vec3 value, const Axis axis) {
  vec3 axis_value;
  vec3_replace_axis(mesh->scale, (float *)value, axis, &axis_value);

  mesh_set_scale(mesh, axis_value);
}

/**
   Apply translation to mesh transform matrix
 */
void mesh_set_position(Mesh *mesh, const vec3 position) {
  glm_vec3_copy((float *)position, mesh->position);

  mesh_update_model_matrix(mesh);

  // update children
  for (size_t i = 0; i < mesh->children.count; i++)
    mesh_set_position(mesh->children.entries[i], position);
}

void mesh_set_position_axis(Mesh *mesh, const vec3 value, const Axis axis) {
  vec3 axis_value;
  vec3_replace_axis(mesh->position, (float *)value, axis, &axis_value);

  mesh_set_position(mesh, axis_value);
}

/**
   Set Euler rotation
 */
void mesh_set_rotation(Mesh *mesh, const vec3 rotation) {
  // cache euler rotation
  glm_vec3_copy((float *)rotation, mesh->rotation_euler);

  // update quat from euler
  vec3 rad_rotation;
  glm_vec3_scale(mesh->rotation_euler, GLM_PI / 180.0f, rad_rotation);

  glm_euler_xyz_quat(rad_rotation, mesh->rotation_quat);

  // recompute model matrix
  mesh_update_model_matrix(mesh);

  // update children
  for (size_t i = 0; i < mesh->children.count; i++)
    mesh_set_rotation(mesh->children.entries[i], rotation);
}

void mesh_set_rotation_axis(Mesh *mesh, const vec3 value, const Axis axis) {
  vec3 axis_value;
  vec3_replace_axis(mesh->rotation_euler, (float *)value, axis, &axis_value);

  mesh_set_rotation(mesh, axis_value);
}

/**
   Apply rotation to mesh transform matrix
 */
void mesh_set_rotation_quat(Mesh *mesh, const versor rotation) {

  // cache quat rotation
  glm_quat_copy((float *)rotation, mesh->rotation_quat);

  // update mesh euler rotation
  mat4 rot;
  glm_quat_mat4(mesh->rotation_quat, rot);

  vec3 rad_euler;
  glm_euler_angles(rot, rad_euler);
  glm_vec3_scale(rad_euler, 180.0f / GLM_PI, mesh->rotation_euler);

  // recompute model matrix
  mesh_update_model_matrix(mesh);

  // update children
  for (size_t i = 0; i < mesh->children.count; i++)
    mesh_set_rotation_quat(mesh->children.entries[i], rotation);
}

/**
   Apply look at transformation to mesh
 */
void mesh_lookat(Mesh *mesh, const vec3 position, const vec3 target) {

  glm_mat4_identity(mesh->model);

  matrix_mesh_lookat(&(UtilsMatrixLookatDescriptor){
      .dest_position = &mesh->position,
      .dest_matrix = &mesh->model,
      .position = (float *)position,
      .target = (float *)target,
      .up = NULL,
      .forward = NULL,
      .right = NULL,
  });
}
