#include "core.h"
#include "./callback.h"
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
  gizmo->active_handle = NULL;

  // define callbacks
  gizmo->transform_callback[GizmoTransformMode_Translate] =
      gizmo_transform_callback_translate;

  gizmo->transform_callback[GizmoTransformMode_Rotate] =
      gizmo_transform_callback_rotate;

  gizmo->transform_callback[GizmoTransformMode_Scale] =
      gizmo_transform_callback_scale;

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
  gizmo->mode = mode;

  // insert new handles
  for (size_t i = 0; i < gizmo->handles[gizmo->mode].length; i++)
    mesh_ref_list_insert(dest_list, gizmo->handles[gizmo->mode].entries[i]);
}

/**
   Search and remove the gizmo handle from the given mode in the destination
   list. Used to make a certain gizmo (translate, rot, scale) disappear in the
   scene (in case the selection went back to 0 as instance).
 */
void gizmo_transform_remove(GizmoTransform *gizmo, MeshRefList *dest_list) {
  for (size_t i = 0; i < gizmo->handles[gizmo->mode].length; i++)
    mesh_ref_list_remove(dest_list, gizmo->handles[gizmo->mode].entries[i]);
}

/**
   Transform handle, used to set the handles at the center of selection.
 */

void gizmo_transform_translate(GizmoTransform *gizmo, vec3 position) {
  mesh_ref_list_translate(&gizmo->handles[gizmo->mode], position);
}

void gizmo_transform_translate_add(GizmoTransform *gizmo, float value,
                                   const Axis axis) {
  mesh_ref_list_translate_axis_add(&gizmo->handles[gizmo->mode], value, axis);
}

void gizmo_transform_rotate_add(GizmoTransform *gizmo, float value,
                                const Axis axis) {
  mesh_ref_list_rotate_axis_add(&gizmo->handles[gizmo->mode], value, axis);
}

/**
   Search the mesh in the gizmo active handles, depending on the mesh index the
   axis is defined (0 = X, 1 = Y, 2 = Z).

   Function prmarily used in raycast selection to retrieve the axis depending on
   the clicked gizmo arrow/ scale or rotation handle.
 */
void gizmo_transform_set_axis_from_mesh(GizmoTransform *gizmo,
                                        const Mesh *mesh) {

  for (size_t j = 0; j < 3; j++) // axis
    if (gizmo->handles[gizmo->mode].entries[j] == mesh)
      gizmo->axis = j;
}

void gizmo_transform_set_active(GizmoTransform *gizmo) {
  gizmo->active_handle = &gizmo->handles[gizmo->mode];
}

void gizmo_transform_clear_active(GizmoTransform *gizmo) {
  gizmo->active_handle = NULL;
}
