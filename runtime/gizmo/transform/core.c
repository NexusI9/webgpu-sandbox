#include "core.h"
#include "./rotate.h"
#include "./scale.h"
#include "./translate.h"
#include "./utils.h"

/**
   Create the three key transform gizmo handles (translate, rotate, scale) and
   set active handle.
 */
void gizmo_transform_create(GizmoTransform *gizmo,
                            const GizmoCreateDescriptor *desc) {

  gizmo->mode = GizmoTransformMode_Translate;
  gizmo->active_handle = &gizmo->handles[gizmo->mode];

  // translate
  gizmo_transform_translate_create(
      &gizmo->handles[GizmoTransformMode_Translate], desc);

  // rotate
  gizmo_transform_translate_create(&gizmo->handles[GizmoTransformMode_Rotate],
                                   desc);

  // scale
  gizmo_transform_scale_create(&gizmo->handles[GizmoTransformMode_Scale], desc);
}

/**
   Update function handles adding and removie the gizmo meshes from the
   destination list based on the mode.
   Since we can only have one gizmo at a time, automating this process prevent
   mistakes by adding multiple times the same gizmo or having two different ones
   at the same time.
 */
void gizmo_transform_update_mode(GizmoTransform *gizmo, MeshRefList *dest_list,
                                 GizmoTransformMode mode) {

  // search & remove active handles from the list
  gizmo_transform_remove(gizmo, dest_list);

  // update active handle & mode
  gizmo->active_handle = &gizmo->handles[mode];
  gizmo->mode = mode;

  // insert new handles
  for (size_t i = 0; i < gizmo->active_handle->length; i++)
    mesh_reference_list_insert(dest_list, gizmo->active_handle->entries[i]);
}

/**
   Search and remove the gizmo handle from the given mode in the destination
   list. Used to make a certain gizmo (translate, rot, scale) disappear in the
   scene (in case the selection went back to 0 as instance).
 */
void gizmo_transform_remove(GizmoTransform *gizmo, MeshRefList *dest_list) {
  for (size_t i = 0; i < gizmo->active_handle->length; i++)
    mesh_reference_list_remove(dest_list, gizmo->active_handle->entries[i]);
}

/**
   Transform handle, used to set the handles at the center of selection.
 */
void gizmo_transform_translate(GizmoTransform *gizmo, vec3 position) {

  mesh_reference_list_translate(gizmo->active_handle, position);
}

void gizmo_transform_rotate(GizmoTransform *gizmo, vec3 rotation) {

  mesh_reference_list_rotate(gizmo->active_handle, rotation);
}
