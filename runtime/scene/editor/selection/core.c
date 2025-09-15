#include "core.h"

#include <cglm/util.h>
#include <cglm/vec3.h>
#include <emscripten/html5.h>
#include <stdbool.h>
#include <stdint.h>

#include "./callback/key.h"
#include "./callback/mouse.h"
#include "./config.h"
#include "./filter.h"
#include "emscripten/em_types.h"
#include "target_list.h"
#include "../runtime/mesh/ref_list.h"
#include "../utils/vector/vec3_list.h"
#include "../runtime/mesh/core.h"
#include "../runtime/scene/core.h"

void scene_selection_init_filters(Scene *scene);

/**
   Initialize the selection functionality on the scene main camera, meaning
   when a mesh is clicked, it displays the transform gizmo.
 */
void scene_selection_init(Scene *scene) {

  // configure editor selections list (fixed)
  scene_selection_init_filters(scene);

  // init selection mouse events
  scene_selection_init_mouse_events(scene);

  // init selection keyboard events
  scene_selection_init_key_events(scene);
}

/**
   To transform the selected meshes and gizmo we poll the mouse event and check
   if the selection pipeline has length.

   Basically our camera raycast/ html events are only used to:
     1. push/pop mesh from the selection array (on right click)
     2. update the gizmo transform active axis (on left click)

    We then constantly through the loop:
     1. check if the mouse is pressed

    According to those checkes we then transform the meshes.

 */
void scene_selection_draw_callback(void *data) {

  Scene *scene = (Scene *)data;
  SceneSelection *selection = &scene->editor.selection;
  Gizmo *gizmo = &scene->editor.gizmo.transform;

  if (gizmo->cache.init_distance != 0.0f) {

    // use each selection filters transform callbacks on their respective meshes
    for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {

      vec3 delta;

      // 1. transform gizmo
      gizmo_transform_callback gizmo_transform_callback =
          gizmo->transform_callback[gizmo->mode];

      gizmo_transform_callback(gizmo, scene->active_camera, &scene->viewport,
                               &delta);

      // 2. transform filter selection with delta calculated by gizmo
      SceneSelectionFilter *filter = &selection->filters[i];

      // look-up filter transform callback depending on gizmo mode
      // (loc/rot/scale)
      scene_selection_transform_callback mesh_transform_callback =
          filter->transform_callback;

      mesh_transform_callback(&(SceneSelectionTransform){
          .active_meshes = &filter->meshes[SceneSelectionState_Selected],
          .target_list = &filter->targets[SceneSelectionState_Selected],
          .initial_attributes = &filter->initial_attributes,
          .delta = &delta,
          .axis = gizmo->axis,
          .transform_mode = gizmo->mode,
          .scene = scene});

      gizmo_update_ssbo(gizmo, &scene->renderer.ssbo);
    }
  }
}

/**
   Define selection rules for each selection lists (mesh or shader-based
   highlight). Configure the included and exclude the mesh reference lists from
   the scene to know which objects can be selected.

   Note that both selection lists mutually exclude each others so they do not
   interfere.
 */
void scene_selection_init_filters(Scene *scene) {

  scene_selection_config(scene);

  // create filters source list
  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {

    SceneSelectionFilter *filter = &scene->editor.selection.filters[i];

    for (size_t j = 0; j < SCENE_SELECTION_STATE_COUNT; j++) {
      // init active list
      mesh_ref_list_create(&filter->meshes[j], MESH_REF_LIST_CAPACITY);

      // init target list
      scene_selection_target_list_create(&filter->targets[j],
                                         MESH_REF_LIST_CAPACITY);
    }

    // init initial attribute list
    vec3_list_create(&filter->initial_attributes, MESH_REF_LIST_CAPACITY);
  }
}

/**
   Add mesh to the selection list
 */
void scene_selection_add(MeshRefList *list, Mesh *mesh) {

  // only add if mesh not already exists
  if (mesh_ref_list_find(list, mesh, NULL) == NULL)
    mesh_ref_list_insert(list, mesh);
}

/**
   Set the gizmo active handle to NULL which acts as a trigger.
   This wall the loop callback doesn't move the meshes anymore if the mouse is
   down again.
 */
bool scene_selection_reset_callback(int eventType,
                                    const EmscriptenMouseEvent *mouseEvent,
                                    void *userData) {

  Scene *scene = (Scene *)userData;

  // clear gizmo cache
  Gizmo *gizmo = &scene->editor.gizmo.transform;
  gizmo_clear_active(gizmo);

  // reset selection initial cached attributes
  scene_selection_empty_initial_attributes(&scene->editor.selection);

  return EM_FALSE;
}

/**
   Get the average position of all selected mesh in all filters.
 */
void scene_selection_average_position(SceneSelection *selection, vec3 *dest) {

  glm_vec3_zero(*dest);

  uint8_t denom = 0;

  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {
    SceneSelectionFilter *filter = &selection->filters[i];
    vec3 filter_avg;
    mesh_ref_list_average_position(
        &filter->meshes[SceneSelectionState_Selected], &filter_avg);
    glm_vec3_add(*dest, filter_avg, *dest);

    if (filter->meshes[SceneSelectionState_Selected].length > 0)
      denom++;
  }

  glm_vec3_scale(*dest, 1.0f / glm_max(denom, 1), *dest);
}

void scene_selection_meshes_lists(SceneSelection *selection,
                                  MeshRefList *list[SCENE_SELECTION_TYPE_COUNT],
                                  size_t *length) {

  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++)
    list[i] = &selection[i].filters->meshes[SceneSelectionState_Selected];

  *length = SCENE_SELECTION_TYPE_COUNT;
}

size_t scene_selection_length(SceneSelection *selection) {

  size_t length = 0;
  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++)
    length += selection->filters[i].meshes[SceneSelectionState_Selected].length;

  return length;
}

/**
   Empty each filter's selection
 */
void scene_selection_empty(SceneSelection *selection) {

  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {
    SceneSelectionFilter *filter = &selection->filters[i];
    scene_selection_filter_set_all_inactive(filter);
  }
}

/**
   Select all objects in each filters and update their highlight callbacks
 */
void scene_selection_all(SceneSelection *selection) {
  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {
    SceneSelectionFilter *filter = &selection->filters[i];
    scene_selection_filter_set_all_active(filter);
  }
}

/**
   Map Gizmo mode to mesh get attributes to apply correct transformation based
   in gizmo mode (trans/rot/scale).

   Used in the selection events when we need to cache the mesh attribute
   (loc/rot/scale) depending on the gizmo mode.
 */
static const mesh_get_transform_attribute mesh_transform_attribute[] = {
    [GizmoMode_Position] = mesh_get_position,
    [GizmoMode_Rotation] = mesh_get_rotation_euler,
    [GizmoMode_Scale] = mesh_get_scale,
};

/**
  Cache all meshes initial attribute based on gizmo mode (pos/rot/scale)
 */
void scene_selection_cache_initial_attributes(SceneSelection *selection,
                                              const GizmoMode mode) {

  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {

    SceneSelectionFilter *filter = &selection->filters[i];

    for (size_t j = 0; j < filter->meshes[SceneSelectionState_Selected].length;
         j++) {

      Mesh *mesh = filter->meshes[SceneSelectionState_Selected].entries[j];
      vec3 attribute;
      mesh_transform_attribute[mode](mesh, &attribute);
      vec3_list_insert(&filter->initial_attributes, attribute);
    }
  }
}

/**
  Empty all meshes initial attribute based on gizmo mode (pos/rot/scale)
 */
void scene_selection_empty_initial_attributes(SceneSelection *selection) {
  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++)
    vec3_list_empty(&selection->filters[i].initial_attributes);
}

void scene_selection_add_mesh(SceneSelection *selection, Mesh *mesh,
                              void *extra, const SceneSelectionType type) {

  // insert mesh to selection meshes
  mesh_ref_list_insert(
      &selection->filters[type].meshes[SceneSelectionState_Default], mesh);

  // push extra
  scene_selection_target_list_insert(
      &selection->filters[type].targets[SceneSelectionState_Default], extra);
}

void scene_selection_add_mesh_ref_list(SceneSelection *selection,
                                       MeshRefList *list, void *extra,
                                       const SceneSelectionType type) {

  for (size_t i = 0; i < list->length; i++) {
    scene_selection_add_mesh(selection, list->entries[i], extra, type);
  }
}
