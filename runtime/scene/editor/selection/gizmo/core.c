#include "core.h"
#include "../runtime/mesh/shader/shader.h"
#include "./callback.h"
#include "./rotate.h"
#include "./scale.h"
#include "./translate.h"
#include "./utils.h"
#include <stddef.h>

static const gizmo_transform_create_handles_callback handles_create_func[] = {
    [GizmoTransformMode_Position] = gizmo_transform_position_create,
    [GizmoTransformMode_Rotation] = gizmo_transform_rotation_create,
    [GizmoTransformMode_Scale] = gizmo_transform_scale_create,
};

static const gizmo_transform_callback transform_callback_func[] = {
    [GizmoTransformMode_Position] = gizmo_transform_callback_position,
    [GizmoTransformMode_Rotation] = gizmo_transform_callback_rotation,
    [GizmoTransformMode_Scale] = gizmo_transform_callback_scale,
};

/**
   Create the three key transform gizmo handles (translate, rotate, scale) and
   set active handle.
 */
void gizmo_transform_create(GizmoTransform *gizmo,
                            const GizmoCreateDescriptor *desc) {

  gizmo->mode = GizmoTransformMode_Position;

  // init 'cache' attributes

  // Use for-loop and lookup tables to map the callbacks functions and creating
  // methods since all handles use the same approach.
  // 0 = Transform, 1 = Rotate, 2 = Scale
  for (size_t i = 0; i < GIZMO_TRANSFORM_AXIS_COUNT; i++) {

    // look up transform callbacks that will be called when a handle will be
    // clicked on
    gizmo->transform_callback[i] = transform_callback_func[i];

    // create handles (mesh / mesh axis)
    handles_create_func[i](&gizmo->handles[i], &gizmo->interactive_handles[i],
                           desc);
  }
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
  mesh_ref_list_transfert(&gizmo->handles[gizmo->mode], dest_list, NULL);
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

void gizmo_transform_set_position(GizmoTransform *gizmo, vec3 position) {
  mesh_ref_list_set_position(&gizmo->handles[gizmo->mode], position);
}

void gizmo_transform_set_rotation_add(GizmoTransform *gizmo, vec3 value,
                                const Axis axis) {
  mesh_ref_list_set_rotation_axis(&gizmo->handles[gizmo->mode], value, axis);
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
    if (gizmo->interactive_handles[gizmo->mode].entries[j] == mesh)
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

   Since the transformation is based on a Delta factor, we need to store initial
   values on click such as "angle" or "initial delta" to calculate the correct
   offset.

 */
void gizmo_transform_set_active(GizmoTransform *gizmo, Camera *camera,
                                Viewport *viewport) {

  // cache gizmo init position
  gizmo_transform_origin(gizmo, &gizmo->cache.gizmo_init_position);

  // cache axis
  // get direction from camera
  if (gizmo->axis == Axis_View) {
    glm_vec3_copy(camera->forward, gizmo->cache.axis_direction);
  } else {
    // get world direction from axis
    vec_world_axis(gizmo->axis, &gizmo->cache.axis_direction);
  }

  // init delta
  Raycast raycast;
  raycast_project_from_screen(
      &raycast, gizmo->axis,
      &(RaycastProjectScreenToAxis){
          .origin = &camera->position,
          .target = &gizmo->cache.gizmo_init_position,
          .axis_direction = &gizmo->cache.axis_direction,
          .view = &camera->view,
          .projection = &viewport->projection,
          .x = g_input.mouse.x,
          .y = g_input.mouse.y,
          .width = viewport->width,
          .height = viewport->height,
      },
      &gizmo->cache.init_delta);

  // define init distance
  gizmo->cache.init_distance = glm_vec3_distance(
      gizmo->cache.gizmo_init_position, gizmo->cache.init_delta);

  // cache inverted distance (since div is expensive)
  gizmo->cache.init_inv_distance = 1.0f / gizmo->cache.init_distance;

  // rotation => angle based, so need to project ray to an infinite plane
  if (gizmo->mode == GizmoTransformMode_Rotation) {

    // init plane
    inf_plane_create(&gizmo->cache.plane, gizmo->cache.gizmo_init_position,
                     gizmo->cache.axis_direction);

    // update init delta
    raycast_hit_inf_plane(&raycast, &gizmo->cache.plane,
                          &gizmo->cache.init_delta);
  }
}

/**
   Clear gizmo cached data. Used on HTML events mouse up so
   during the next mouse down we can repopulate the new data.
 */
void gizmo_transform_clear_active(GizmoTransform *gizmo) {
  // reset gizmo initial position and delta
  glm_vec3_copy(GLM_VEC3_ZERO, gizmo->cache.gizmo_init_position);
  glm_vec3_copy(GLM_VEC3_ZERO, gizmo->cache.init_delta);
  gizmo->cache.init_distance = 0.0f;
}

/**
   Got through the active meshes and update their uniform back to their default
   one. Function primarily used in the selection callback to set back the handle
   color on mouse leave.

   Use a lookup table coupled with a linear search to pick the right pointer.
 */
void gizmo_transform_reset_color_uniform(GizmoTransform *gizmo) {

  for (uint8_t i = 0; i < GIZMO_TRANSFORM_AXIS_COUNT; i++) {
    Mesh *handle = gizmo->interactive_handles[gizmo->mode].entries[i];
    shader_update_uniform(mesh_shader(handle, MeshShader_Fixed), 1, 0,
                          gizmo_handle_color[i]);
  }
}
