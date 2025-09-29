#include "core.h"

#include <cglm/util.h>
#include <cglm/vec3.h>
#include <emscripten/html5.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "./callback/key.h"
#include "./callback/mouse.h"
#include "./config.h"
#include "./filter.h"
#include "emscripten/em_types.h"
#include "runtime/input/core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/selection/utils.h"
#include "target_list.h"
#include "utils/dyli.h"
#include "utils/vector/vec3_list.h"

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
    for (SceneSelectionType i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {

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
 
      mesh_transform_callback(
          &(SceneSelectionTransform){.selection = &filter->selection,
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
  for (SceneSelectionType i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {
    SceneSelectionFilter *filter = &scene->editor.selection.filters[i];

    // init active list
    mesh_ref_list_create(&filter->meshes, MESH_REF_LIST_CAPACITY);

    // init target list
    scene_selection_target_list_create(&filter->targets,
                                       MESH_REF_LIST_CAPACITY);

    dyli_create((void *)&filter->selection.entries, &filter->selection.capacity,
                &filter->selection.length, sizeof(SceneSelectionObject),
                MESH_REF_LIST_CAPACITY, "Scene Selection Object List");
  }
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
  scene_selection_clear_initial_attributes(&scene->editor.selection);

  return EM_FALSE;
}

/**
   Get the average position of all selected mesh in all filters.
 */
void scene_selection_average_position(SceneSelection *selection, vec3 *dest) {

  glm_vec3_zero(*dest);

  uint8_t denom = 0;

  for (SceneSelectionType i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {
    SceneSelectionFilter *filter = &selection->filters[i];
    vec3 filter_avg;
    glm_vec3_zero(filter_avg);

    for (size_t j = 0; j < filter->selection.length; j++) {
      SceneSelectionObject *object = &filter->selection.entries[j];
      glm_vec3_add(object->mesh->position, filter_avg, filter_avg);
    }
    glm_vec3_scale(filter_avg, 1.0f / glm_max(filter->selection.length, 1),
                   filter_avg);

    glm_vec3_add(*dest, filter_avg, *dest);

    if (filter->selection.length > 0)
      denom++;
  }

  glm_vec3_scale(*dest, 1.0f / glm_max(denom, 1), *dest);
}

size_t scene_selection_length(SceneSelection *selection) {
  size_t length = 0;
  for (SceneSelectionType i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++)
    length += selection->filters[i].selection.length;

  return length;
}

/**
   Empty each filter's selection
 */
void scene_selection_empty(SceneSelection *selection) {

  for (SceneSelectionType i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {
    SceneSelectionFilter *filter = &selection->filters[i];
    scene_selection_filter_set_all_inactive(filter);
  }
}

/**
   Select all objects in each filters and update their highlight callbacks
 */
void scene_selection_all(SceneSelection *selection) {
  for (SceneSelectionType i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {
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

    for (size_t j = 0; j < filter->selection.length; j++) {
      SceneSelectionObject *object = &filter->selection.entries[j];
      vec3 attribute;
      mesh_transform_attribute[mode](object->mesh, &attribute);
      glm_vec3_copy(attribute, object->initial_attribute);
    }
  }
}

/**
  Empty all meshes initial attribute based on gizmo mode (pos/rot/scale)
 */
void scene_selection_clear_initial_attributes(SceneSelection *selection) {
  for (SceneSelectionType i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++)
    for (size_t j = 0; j < selection->filters[i].selection.length; j++)
      glm_vec3_zero(
          selection->filters[i].selection.entries[j].initial_attribute);
}

/**
   Add a mesh to a specific selection pipeline (= type).
   Each type/pipeline has its own highlight and transform callback.
   Currently we defined 3 types:
   - Mesh Shadow
   - Mesh
   - Scene Editor Objects (SEO)
 */
void scene_selection_subscribe_mesh(SceneSelection *selection, Mesh *mesh,
                                    scene_selection_target_t extra,
                                    const SceneSelectionType type) {

  // insert mesh to selection meshes
  mesh_ref_list_insert(&selection->filters[type].meshes, mesh);

  // push extra
  SceneSelectionTargetList *target_list = &selection->filters[type].targets;

  scene_selection_target_t target =
      extra != NULL ? extra : &(scene_selection_target_t){0};

  scene_selection_target_list_insert(target_list, target);
}

void scene_selection_subscribe_mesh_ref_list(SceneSelection *selection,
                                             MeshRefList *list, void *extra,
                                             const SceneSelectionType type) {
  for (size_t i = 0; i < list->length; i++)
    scene_selection_subscribe_mesh(selection, list->entries[i], extra, type);
}

/**
   Handle the overall flow of selection state, including:
   - Add mesh to filter selection list
   - Trigger highlight callback
   - Handle the gizmo visibility

   This function is used as the main function to add/remove mesh from the
   selection depending on the trigger method (click, shortcut, UI)
 */
void scene_selection_update_mesh(Scene *scene, Mesh *mesh) {

  bool selected;
  SceneSelectionFilter *filter = scene_selection_filter_find_mesh(
      &scene->editor.selection, mesh, &selected);

  if (filter == NULL)
    return;

  if (!selected) {

    if (input_key(INPUT_KEY_CAP) == false)
      scene_selection_empty(&scene->editor.selection);

    scene_selection_filter_selection_add_mesh(filter, mesh, NULL);
  } else {
    scene_selection_filter_selection_remove_mesh(filter, mesh);
  }

  // update highlight
  if (filter->highlight_callback)
    filter->highlight_callback(&filter->meshes, &filter->selection, scene);

  // handle gizmo
  if (scene_selection_length(&scene->editor.selection) > 0) {
    scene_gizmo_pos_to_selection(&scene->editor.gizmo.transform,
                                 &scene->editor.selection,
                                 &scene->renderer.ssbo);
    scene_gizmo_show(scene);

  } else {
    scene_gizmo_hide(scene);
  }
}
