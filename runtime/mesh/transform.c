#include "transform.h"
#include "../utils/matrix.h"


// callbacks look up

// translate look up
static const mesh_transform_uni_axis_callback mesh_translate_axis_callback[] = {
    [Axis_X] = mesh_translate_x,
    [Axis_Y] = mesh_translate_y,
    [Axis_Z] = mesh_translate_z,
};

// rotate look up
static const mesh_transform_uni_axis_callback mesh_rotate_axis_callback[] = {
    [Axis_X] = mesh_rotate_x,
    [Axis_Y] = mesh_rotate_y,
    [Axis_Z] = mesh_rotate_z,
};

// scale look up
static const mesh_transform_uni_axis_callback mesh_scale_axis_callback[] = {
    [Axis_X] = mesh_scale_x,
    [Axis_Y] = mesh_scale_y,
    [Axis_Z] = mesh_scale_z,
};

// translate add look up
static const mesh_transform_uni_axis_callback mesh_translate_axis_add_callback[] = {
    [Axis_X] = mesh_translate_x_add,
    [Axis_Y] = mesh_translate_y_add,
    [Axis_Z] = mesh_translate_z_add,
};

// rotate add look up
static const mesh_transform_uni_axis_callback mesh_rotate_axis_add_callback[] = {
    [Axis_X] = mesh_rotate_x_add,
    [Axis_Y] = mesh_rotate_y_add,
    [Axis_Z] = mesh_rotate_z_add,
};

// scale add look up
static const mesh_transform_uni_axis_callback mesh_scale_axis_add_callback[] = {
    [Axis_X] = mesh_scale_x_add,
    [Axis_Y] = mesh_scale_y_add,
    [Axis_Z] = mesh_scale_z_add,
};



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
                                &mesh->topology.boundbox, mesh->queue);
}

/**
   Apply scale to mesh transform matrix
 */
void mesh_scale(Mesh *mesh, vec3 scale) {
  glm_vec3_copy(scale, mesh->scale);

  mesh_update_model_matrix(mesh);
}

void mesh_scale_axis(Mesh *mesh, const float value, const Axis axis) {
  mesh_scale_axis_callback[axis](mesh, value);
}

void mesh_scale_axis_add(Mesh *mesh, const float value, const Axis axis) {
  mesh_scale_axis_add_callback[axis](mesh, value);
}

void mesh_scale_x(Mesh *mesh, const float value) {
  mesh_scale(mesh, (vec3){
                       value,
                       mesh->scale[1],
                       mesh->scale[2],
                   });
}

void mesh_scale_y(Mesh *mesh, const float value) {
  mesh_scale(mesh, (vec3){
                       mesh->scale[0],
                       value,
                       mesh->scale[2],
                   });
}

void mesh_scale_z(Mesh *mesh, const float value) {
  mesh_scale(mesh, (vec3){
                       mesh->scale[0],
                       mesh->scale[1],
                       value,
                   });
}

void mesh_scale_x_add(Mesh *mesh, const float value) {
  mesh_scale(mesh, (vec3){
                       mesh->scale[0] + value,
                       mesh->scale[1],
                       mesh->scale[2],
                   });
}

void mesh_scale_y_add(Mesh *mesh, const float value) {
  mesh_scale(mesh, (vec3){
                       mesh->scale[0],
                       mesh->scale[1] + value,
                       mesh->scale[2],
                   });
}

void mesh_scale_z_add(Mesh *mesh, const float value) {
  mesh_scale(mesh, (vec3){
                       mesh->scale[0],
                       mesh->scale[1],
                       mesh->scale[2] + value,
                   });
}

/**
   Apply translation to mesh transform matrix
 */
void mesh_translate(Mesh *mesh, vec3 position) {
  glm_vec3_copy(position, mesh->position);

  mesh_update_model_matrix(mesh);
}

void mesh_translate_axis(Mesh *mesh, const float value, const Axis axis) {
  mesh_translate_axis_callback[axis](mesh, value);
}

void mesh_translate_axis_add(Mesh *mesh, const float value,
                             const Axis axis) {
  mesh_translate_axis_add_callback[axis](mesh, value);
}

void mesh_translate_x(Mesh *mesh, const float value) {
  mesh_translate(mesh, (vec3){
                           value,
                           mesh->position[1],
                           mesh->position[2],
                       });
}

void mesh_translate_y(Mesh *mesh, const float value) {
  mesh_translate(mesh, (vec3){
                           mesh->position[0],
                           value,
                           mesh->position[2],
                       });
}

void mesh_translate_z(Mesh *mesh, const float value) {
  mesh_translate(mesh, (vec3){
                           mesh->position[0],
                           mesh->position[1],
                           value,
                       });
}

void mesh_translate_x_add(Mesh *mesh, const float value) {
  mesh_translate(mesh, (vec3){
                           mesh->position[0] + value,
                           mesh->position[1],
                           mesh->position[2],
                       });
}

void mesh_translate_y_add(Mesh *mesh, const float value) {
  mesh_translate(mesh, (vec3){
                           mesh->position[0],
                           mesh->position[1] + value,
                           mesh->position[2],
                       });
}

void mesh_translate_z_add(Mesh *mesh, const float value) {
  mesh_translate(mesh, (vec3){
                           mesh->position[0],
                           mesh->position[1],
                           mesh->position[2] + value,
                       });
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

void mesh_rotate_axis(Mesh *mesh, const float value, const Axis axis) {
  mesh_rotate_axis_callback[axis](mesh, value);
}

void mesh_rotate_axis_add(Mesh *mesh, const float value, const Axis axis) {
  mesh_rotate_axis_add_callback[axis](mesh, value);
}

void mesh_rotate_x(Mesh *mesh, const float value) {
  mesh_rotate(mesh, (vec3){
                        value,
                        mesh->rotation_euler[1],
                        mesh->rotation_euler[2],
                    });
}

void mesh_rotate_y(Mesh *mesh, const float value) {
  mesh_rotate(mesh, (vec3){
                        mesh->rotation_euler[0],
                        value,
                        mesh->rotation_euler[2],
                    });
}

void mesh_rotate_z(Mesh *mesh, const float value) {
  mesh_rotate(mesh, (vec3){
                        mesh->rotation_euler[0],
                        mesh->rotation_euler[1],
                        value,
                    });
}

void mesh_rotate_x_add(Mesh *mesh, const float value) {
  mesh_rotate(mesh, (vec3){
                        mesh->rotation_euler[0] + value,
                        mesh->rotation_euler[1],
                        mesh->rotation_euler[2],
                    });
}

void mesh_rotate_y_add(Mesh *mesh, const float value) {
  mesh_rotate(mesh, (vec3){
                        mesh->rotation_euler[0],
                        mesh->rotation_euler[1] + value,
                        mesh->rotation_euler[2],
                    });
}

void mesh_rotate_z_add(Mesh *mesh, const float value) {
  mesh_rotate(mesh, (vec3){
                        mesh->rotation_euler[0],
                        mesh->rotation_euler[1],
                        mesh->rotation_euler[2] + value,
                    });
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
