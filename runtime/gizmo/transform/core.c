#include "core.h"
#include "./rotate.h"
#include "./scale.h"
#include "./translate.h"
#include "./utils.h"

static void gizmo_transform_insert(GizmoTransform *, MeshRefList *,
                                   GizmoTransformMode);

static void gizmo_transform_remove(GizmoTransform *, MeshRefList *,
                                   GizmoTransformMode);

/**
   Create the three key transform gizmo handles (translate, rotate, scale) and
   set active handle.
 */
void gizmo_transform_create(GizmoTransform *gizmo,
                            const GizmoCreateDescriptor *desc) {

  gizmo->active_handle = &gizmo->handles[GizmoTransformMode_Translate];

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
void gizmo_transform_update(GizmoTransform *gizmo, MeshRefList *dest_list,
                            GizmoTransformMode mode) {

  // search & remove active handles from the list
  for (size_t i = 0; i < gizmo->active_handle->length; i++)
    mesh_reference_list_remove(dest_list, gizmo->active_handle->entries[i]);

  // update active handle & mode
  gizmo->active_handle = &gizmo->handles[mode];
  gizmo->mode = mode;

  // insert new handles
  for (size_t i = 0; i < gizmo->active_handle->length; i++)
    mesh_reference_list_insert(dest_list, gizmo->active_handle->entries[i]);
}

/**
   Insert the gizmo handle from the given mode in the destination list.
   Used to make a certain gizmo (translate, rot, scale) appear in the scene.
 */
void gizmo_transform_insert(GizmoTransform *gizmo, MeshRefList *dest_list,
                            GizmoTransformMode mode) {}

/**
   Search and remove the gizmo handle from the given mode in the destination
   list. Used to make a certain gizmo (translate, rot, scale) disappear in the
   scene.
 */
void gizmo_transform_remove(GizmoTransform *gizmo, MeshRefList *dest_list,
                            GizmoTransformMode mode) {}
