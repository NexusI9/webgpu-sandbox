#include "ref_list_transform.h"
#include "transform.h"

/**
   Apply scale to mesh transform matrix
 */
void mesh_ref_list_scale(MeshRefList *list, vec3 scale) {
  for (size_t i = 0; i < list->length; i++)
    mesh_scale(list->entries[i], scale);
}

void mesh_ref_list_scale_axis(MeshRefList *list, vec3 value,
                              const Axis axis) {
  for (size_t i = 0; i < list->length; i++)
    mesh_scale_axis(list->entries[i], value, axis);
}

/**
   Apply translation to mesh transform matrix
 */
void mesh_ref_list_translate(MeshRefList *list, vec3 position) {
  for (size_t i = 0; i < list->length; i++)
    mesh_translate(list->entries[i], position);
}

void mesh_ref_list_translate_axis(MeshRefList *list, vec3 value,
                                  const Axis axis) {
  for (size_t i = 0; i < list->length; i++)
    mesh_translate_axis(list->entries[i], value, axis);
}


/**
   Set Euler rotation
 */
void mesh_ref_list_rotate(MeshRefList *list, vec3 rotation) {
  for (size_t i = 0; i < list->length; i++)
    mesh_rotate(list->entries[i], rotation);
}

void mesh_ref_list_rotate_axis(MeshRefList *list, vec3 value,
                               const Axis axis) {
  for (size_t i = 0; i < list->length; i++)
    mesh_rotate_axis(list->entries[i], value, axis);
}


void mesh_ref_list_lookat(MeshRefList *list, vec3 position, vec3 target) {
  for (size_t i = 0; i < list->length; i++)
    mesh_lookat(list->entries[i], position, target);
}
