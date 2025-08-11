#include "transform.h"

static const mesh_transform_callback transform_callback_mesh[] = {
    [GizmoTransformMode_Translate] = mesh_translate,
    [GizmoTransformMode_Rotate] = mesh_rotate,
    [GizmoTransformMode_Scale] = mesh_scale,
};

/* Mesh based transform */
void scene_selection_mesh_transform(MeshRefList *active_meshes,
                                    SceneSelectionTargetList *target_list,
                                    Vec3List *initial_attributes, vec3 delta,
                                    const Axis axis,
                                    const GizmoTransformMode transform_mode,
                                    Scene *scene) {
  for (size_t i = 0; i < active_meshes->length; i++) {

    vec3 *init_attribute = &initial_attributes->entries[i];
    Mesh *mesh = active_meshes->entries[i];

    // calculate offset from delta
    vec3 offset_attribute;
    glm_vec3_add(*init_attribute, delta, offset_attribute);

    // transform mesh
    transform_callback_mesh[transform_mode](mesh, offset_attribute);

    // TODO: update shadow map

  }
}

/* SEO based transform */
void scene_selection_seo_transform(MeshRefList *active_meshes,
                                   SceneSelectionTargetList *target_list,
                                   Vec3List *initial_attributes, vec3 delta,
                                   const Axis axis,
                                   const GizmoTransformMode transform_mode,
                                   Scene *scene) {

  for (size_t i = 0; i < active_meshes->length; i++) {

    vec3 *init_attribute = &initial_attributes->entries[i];
    Mesh *mesh = active_meshes->entries[i];
    SceneEditorObject *seo = (SceneEditorObject *)target_list->entries[i];

    // calculate offset from delta
    vec3 offset_attribute;
    glm_vec3_add(*init_attribute, delta, offset_attribute);

    // transform seo via their own callback
    seo->transform_callback[transform_mode](seo, offset_attribute);
  }
}
