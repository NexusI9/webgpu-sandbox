#include "callback_transform.h"

/* Mesh based transform */
void scene_selection_mesh_translate(MeshRefList *list, Vec3List *init_attr,
                                    vec3 delta, const Axis axis) {

  // move meshes
  for (size_t i = 0; i < list->length; i++) {

    vec3 *init_attribute = &init_attr->entries[i];
    Mesh *mesh = list->entries[i];

    // calculate offset
    vec3 offset_attribute;

    glm_vec3_add(*init_attribute, delta, offset_attribute);

    // translate mesh
    mesh_translate_axis(mesh, offset_attribute, axis);
  }
}

void scene_selection_mesh_rotate(MeshRefList *list, Vec3List *init_attr,
                                 vec3 delta, const Axis axis) {

  // rotate meshes
  for (size_t i = 0; i < list->length; i++) {

    vec3 *init_attribute = &init_attr->entries[i];
    Mesh *mesh = list->entries[i];

    // calculate offset
    vec3 offset_attribute;
    glm_vec3_add(*init_attribute, delta, offset_attribute);

    // translate mesh
    mesh_rotate_axis(mesh, offset_attribute, axis);
  }
}
void scene_selection_mesh_scale(MeshRefList *list, Vec3List *init_attr,
                                vec3 delta, const Axis axis) {
  // move meshes
  for (size_t i = 0; i < list->length; i++) {

    vec3 *init_attribute = &init_attr->entries[i];
    Mesh *mesh = list->entries[i];

    // calculate offset
    vec3 offset_attribute;

    glm_vec3_add(*init_attribute, delta, offset_attribute);

    // translate mesh
    mesh_scale_axis(mesh, offset_attribute, axis);
  }
}

/* Shader based transform */
void scene_selection_shader_translate(MeshRefList *list, Vec3List *init_attr,
                                      vec3 delta, const Axis axis) {}

void scene_selection_shader_rotate(MeshRefList *list, Vec3List *init_attr,
                                   vec3 delta, const Axis axis) {}

void scene_selection_shader_scale(MeshRefList *list, Vec3List *init_attr,
                                  vec3 delta, const Axis axis) {}
