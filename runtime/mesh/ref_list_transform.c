#include "ref_list_transform.h"

#include <stddef.h>

#include "transform.h"
#include "utils/vector/core.h"

/**
   Apply scale to mesh transform matrix
 */
void mesh_ref_list_set_scale(MeshRefList *list, vec3 scale) {
  for (size_t i = 0; i < list->length; i++)
    mesh_set_scale(list->entries[i], scale);
}

void mesh_ref_list_set_scale_axis(MeshRefList *list, vec3 value,
                                  const Axis axis) {
  for (size_t i = 0; i < list->length; i++)
    mesh_set_scale_axis(list->entries[i], value, axis);
}

/**
   Apply translation to mesh transform matrix
 */
void mesh_ref_list_set_position(MeshRefList *list, vec3 position) {
  for (size_t i = 0; i < list->length; i++)
    mesh_set_position(list->entries[i], position);
}

void mesh_ref_list_set_position_axis(MeshRefList *list, vec3 value,
                                     const Axis axis) {
  for (size_t i = 0; i < list->length; i++)
    mesh_set_position_axis(list->entries[i], value, axis);
}

/**
   Set Euler rotation
 */
void mesh_ref_list_set_rotation(MeshRefList *list, vec3 rotation) {
  for (size_t i = 0; i < list->length; i++)
    mesh_set_rotation(list->entries[i], rotation);
}

void mesh_ref_list_set_rotation_axis(MeshRefList *list, vec3 value,
                                     const Axis axis) {
  for (size_t i = 0; i < list->length; i++)
    mesh_set_rotation_axis(list->entries[i], value, axis);
}

void mesh_ref_list_lookat(MeshRefList *list, vec3 position, vec3 target) {
  for (size_t i = 0; i < list->length; i++)
    mesh_lookat(list->entries[i], position, target);
}
