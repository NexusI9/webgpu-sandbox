#include "ref_list_transform.h"
#include "transform.h"

/**
   Apply scale to mesh transform matrix
 */
void mesh_ref_list_scale(MeshRefList *list, vec3 scale) {
  for (size_t i = 0; i < list->length; i++)
    mesh_scale(list->entries[i], scale);
}

void mesh_ref_list_scale_axis(MeshRefList *list, const float value,
                              const Axis axis) {
  for (size_t i = 0; i < list->length; i++)
    mesh_scale_axis(list->entries[i], value, axis);
}

void mesh_ref_list_scale_axis_add(MeshRefList *list, const float value,
                                  const Axis axis) {
  for (size_t i = 0; i < list->length; i++)
    mesh_scale_axis_add(list->entries[i], value, axis);
}

void mesh_ref_list_scale_x(MeshRefList *list, const float value) {
  for (size_t i = 0; i < list->length; i++)
    mesh_scale_x(list->entries[i], value);
}

void mesh_ref_list_scale_y(MeshRefList *list, const float value) {
  for (size_t i = 0; i < list->length; i++)
    mesh_scale_y(list->entries[i], value);
}

void mesh_ref_list_scale_z(MeshRefList *list, const float value) {
  for (size_t i = 0; i < list->length; i++)
    mesh_scale_z(list->entries[i], value);
}

void mesh_ref_list_scale_x_add(MeshRefList *list, const float value) {
  for (size_t i = 0; i < list->length; i++)
    mesh_scale_x_add(list->entries[i], value);
}

void mesh_ref_list_scale_y_add(MeshRefList *list, const float value) {
  for (size_t i = 0; i < list->length; i++)
    mesh_scale_y_add(list->entries[i], value);
}

void mesh_ref_list_scale_z_add(MeshRefList *list, const float value) {
  for (size_t i = 0; i < list->length; i++)
    mesh_scale_z_add(list->entries[i], value);
}

/**
   Apply translation to mesh transform matrix
 */
void mesh_ref_list_translate(MeshRefList *list, vec3 position) {
  for (size_t i = 0; i < list->length; i++)
    mesh_translate(list->entries[i], position);
}

void mesh_ref_list_translate_axis(MeshRefList *list, const float value,
                                  const Axis axis) {
  for (size_t i = 0; i < list->length; i++)
    mesh_translate_axis(list->entries[i], value, axis);
}

void mesh_ref_list_translate_axis_add(MeshRefList *list, const float value,
                                      const Axis axis) {
  for (size_t i = 0; i < list->length; i++)
    mesh_translate_axis_add(list->entries[i], value, axis);
}

void mesh_ref_list_translate_x(MeshRefList *list, const float value) {
  for (size_t i = 0; i < list->length; i++)
    mesh_translate_x(list->entries[i], value);
}

void mesh_ref_list_translate_y(MeshRefList *list, const float value) {
  for (size_t i = 0; i < list->length; i++)
    mesh_translate_y(list->entries[i], value);
}

void mesh_ref_list_translate_z(MeshRefList *list, const float value) {
  for (size_t i = 0; i < list->length; i++)
    mesh_translate_z(list->entries[i], value);
}

void mesh_ref_list_translate_x_add(MeshRefList *list, const float value) {
  for (size_t i = 0; i < list->length; i++)
    mesh_translate_x_add(list->entries[i], value);
}

void mesh_ref_list_translate_y_add(MeshRefList *list, const float value) {
  for (size_t i = 0; i < list->length; i++)
    mesh_translate_y_add(list->entries[i], value);
}

void mesh_ref_list_translate_z_add(MeshRefList *list, const float value) {
  for (size_t i = 0; i < list->length; i++)
    mesh_translate_z_add(list->entries[i], value);
}

/**
   Set Euler rotation
 */
void mesh_ref_list_rotate(MeshRefList *list, vec3 rotation) {
  for (size_t i = 0; i < list->length; i++)
    mesh_rotate(list->entries[i], rotation);
}

void mesh_ref_list_rotate_axis(MeshRefList *list, const float value,
                               const Axis axis) {
  for (size_t i = 0; i < list->length; i++)
    mesh_rotate_axis(list->entries[i], value, axis);
}

void mesh_ref_list_rotate_axis_add(MeshRefList *list, const float value,
                                   const Axis axis) {
  for (size_t i = 0; i < list->length; i++)
    mesh_rotate_axis_add(list->entries[i], value, axis);
}

void mesh_ref_list_rotate_x(MeshRefList *list, const float value) {
  for (size_t i = 0; i < list->length; i++)
    mesh_rotate_x(list->entries[i], value);
}

void mesh_ref_list_rotate_y(MeshRefList *list, const float value) {
  for (size_t i = 0; i < list->length; i++)
    mesh_rotate_y(list->entries[i], value);
}

void mesh_ref_list_rotate_z(MeshRefList *list, const float value) {
  for (size_t i = 0; i < list->length; i++)
    mesh_rotate_z(list->entries[i], value);
}

void mesh_ref_list_rotate_x_add(MeshRefList *list, const float value) {
  for (size_t i = 0; i < list->length; i++)
    mesh_rotate_x_add(list->entries[i], value);
}

void mesh_ref_list_rotate_y_add(MeshRefList *list, const float value) {
  for (size_t i = 0; i < list->length; i++)
    mesh_rotate_y_add(list->entries[i], value);
}

void mesh_ref_list_rotate_z_add(MeshRefList *list, const float value) {
  for (size_t i = 0; i < list->length; i++)
    mesh_rotate_z_add(list->entries[i], value);
}

void mesh_ref_list_rotate_quat(MeshRefList *list, versor rotation) {
  for (size_t i = 0; i < list->length; i++)
    mesh_rotate_quat(list->entries[i], rotation);
}

void mesh_ref_list_lookat(MeshRefList *list, vec3 position, vec3 target) {
  for (size_t i = 0; i < list->length; i++)
    mesh_lookat(list->entries[i], position, target);
}
