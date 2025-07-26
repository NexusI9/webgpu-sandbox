#include "core.h"
#include "./callback.h"
#include "./rotate.h"
#include "./scale.h"
#include "./translate.h"
#include "./utils.h"
#include <stddef.h>

// Map Gizmo mode to mesh get attributes to apply correct transformation based
// in gizmo mode (trans/rot/scale).
static const mesh_get_transform_attribute mesh_transform_attribute[] = {
    [GizmoTransformMode_Translate] = mesh_get_position,
    [GizmoTransformMode_Rotate] = mesh_get_rotation_euler,
    [GizmoTransformMode_Scale] = mesh_get_scale,
};

static inline void gizmo_transform_set_axis_from_mesh(GizmoTransform *,
                                                      const Mesh *);

/**
   Create the three key transform gizmo handles (translate, rotate, scale) and
   set active handle.
 */
void gizmo_transform_create(GizmoTransform *gizmo,
                            const GizmoCreateDescriptor *desc) {

  gizmo->mode = GizmoTransformMode_Rotate;

  // init 'cache' attributes
  const size_t capacity = GIZMO_TRANSFORM_POSITION_CAPACITY;
  vec3_list_create(&gizmo->cache.selection_init_attribute, capacity);
  mesh_ref_list_create(&gizmo->cache.selection, capacity);

  // define callbacks that will be called when a handle will be clicked on
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
  gizmo_transform_rotate_create(&gizmo->handles[GizmoTransformMode_Rotate],
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

/**
   Main "activator" for the gizmo transform. Called when user selected meshes
   and clicked on one of the gizmo axes.

   Key operations:
   1. Define gizmo active axis based on the handle clicked on (X/Y/Z)

   2. Define gizmo active handles (trans/rot/scale) based on current gizmo mode.
   Gizmo active handles kinda acts as a trigger to tell the loop check that the
   gizmo is ready to move object during polling.

   3. Finally cache gizmo initial offset position projected on the right axis.

 */
void gizmo_transform_set_active(GizmoTransform *gizmo, const Mesh *hit_handle,
                                const MeshRefList *selected_meshes,
                                Camera *camera, Viewport *viewport) {

  // define active axis
  gizmo_transform_set_axis_from_mesh(gizmo, hit_handle);

  // cache gizmo init position
  gizmo_transform_origin(gizmo, &gizmo->cache.gizmo_init_position);

  // update selection
  mesh_ref_list_transfert(selected_meshes, &gizmo->cache.selection);

  // cache all meshes initial attribute based on gizmo mode (pos/rot/scale)
  for (size_t i = 0; i < selected_meshes->length; i++) {
    Mesh *mesh = selected_meshes->entries[i];
    vec3 attribute;
    mesh_transform_attribute[gizmo->mode](mesh, &attribute);
    vec3_list_insert(&gizmo->cache.selection_init_attribute, attribute);
  }

  // init delta
  vec3 axis_dir;
  vec_world_axis(gizmo->axis, &axis_dir);

  raycast_project_from_screen_to_axis(
      &(RaycastProjectScreenToAxis){
          .origin = &camera->position,
          .target = &gizmo->cache.gizmo_init_position,
          .axis_direction = &axis_dir,
          .view = &camera->view,
          .projection = &viewport->projection,
          .x = g_input.mouse.x,
          .y = g_input.mouse.y,
          .width = viewport->width,
          .height = viewport->height,
      },
      &gizmo->cache.delta_init);
}

/**
   Clear gizmo cached data. Used on HTML events mouse up so
   during the next mouse down we can repopulate the new data.
 */
void gizmo_transform_clear_active(GizmoTransform *gizmo) {
  // reset gizmo initial position and delta
  glm_vec3_copy(GLM_VEC3_ZERO, gizmo->cache.gizmo_init_position);
  glm_vec3_copy(GLM_VEC3_ZERO, gizmo->cache.delta_init);

  // reset cache meshes positions
  vec3_list_empty(&gizmo->cache.selection_init_attribute);

  // empty selection list
  mesh_ref_list_empty(&gizmo->cache.selection);
}
