#include "core.h"
#include "../../show.h"
#include "./callback_key.h"
#include "./callback_mouse.h"
#include "./config.h"
#include "./utils.h"
#include "emscripten/em_types.h"

void scene_selection_init_rules(Scene *scene);

/**
   Initialize the selection functionality on the scene main camera, meaning
   when a mesh is clicked, it displays the transform gizmo.
 */
void scene_selection_init(Scene *scene) {

  // init selection list
  mesh_ref_list_create(&scene->pipelines[ScenePipeline_Fixed_Selection],
                       SCENE_MESH_LIST_DEFAULT_CAPACITY);

  // configure editor selections list (fixed)
  scene_selection_init_rules(scene);

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

  Scene *cast_scene = (Scene *)data;
  SceneSelection *selection = &cast_scene->editor.selection;
  GizmoTransform *gizmo = &cast_scene->editor.gizmo.transform;

  if (gizmo->cache.init_distance != 0.0f) {

    // use each selection filters transform callbacks on their respective meshes
    for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {

      vec3 delta;

      // 1. transform gizmo
      gizmo_transform_callback gizmo_transform_callback =
          gizmo->transform_callback[gizmo->mode];

      gizmo_transform_callback(gizmo, cast_scene->active_camera,
                               &cast_scene->viewport, &delta);

      // 2. transform filter selection with delta calculated by gizmo
      SceneSelectionFilter *filter = &selection->filters[i];

      // look-up filter transform callback depending on gizmo mode
      // (loc/rot/scale)
      scene_selection_transform_callback mesh_transform_callback =
          filter->transform_callbacks[gizmo->mode];

      mesh_transform_callback(&filter->selection, &filter->init_attribute,
                              delta, gizmo->axis);
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
void scene_selection_init_rules(Scene *scene) {

  scene_selection_config(scene);

  // create filters source list
  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {

    SceneSelectionFilter *filter = &scene->editor.selection.filters[i];

    // init selection list
    mesh_ref_list_create(&filter->selection, MESH_REF_LIST_CAPACITY);

    // init initial attribute list
    vec3_list_create(&filter->init_attribute, MESH_REF_LIST_CAPACITY);
  }
}

/**
   Add mesh to the selection list
 */
void scene_selection_add(MeshRefList *list, Mesh *mesh) {

  // only add if mesh not already exists
  if (mesh_ref_list_find(list, mesh) == NULL)
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
  GizmoTransform *gizmo = &scene->editor.gizmo.transform;
  gizmo_transform_clear_active(gizmo);

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
    mesh_ref_list_average_position(&filter->selection, &filter_avg);
    glm_vec3_add(*dest, filter_avg, *dest);

    if (filter->selection.length > 0)
      denom++;
  }

  glm_vec3_scale(*dest, 1.0f / glm_max(denom, 1), *dest);
}

void scene_selection_meshes_lists(SceneSelection *selection,
                                  MeshRefList *list[SCENE_SELECTION_TYPE_COUNT],
                                  size_t *length) {

  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++)
    list[i] = &selection[i].filters->selection;

  *length = SCENE_SELECTION_TYPE_COUNT;
}

size_t scene_selection_length(SceneSelection *selection) {

  size_t length = 0;
  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++)
    length += selection->filters[i].selection.length;

  return length;
}

bool scene_selection_filter_include_mesh(SceneSelectionFilter *filter,
                                         Mesh *mesh) {
  bool included = false;
  // check include
  for (size_t j = 0; j < filter->include.length; j++) {

    MeshRefList *include_list = filter->include.entries[j];

    Mesh *find = mesh_ref_list_find(include_list, mesh);

    // if mesh found in current include list
    if (find != NULL) {
      included = true;

      // if no exclude, no need to check anymore
      if (filter->exclude.length == 0)
        break;
    }
  }

  // check exclude
  for (size_t k = 0; k < filter->exclude.length; k++) {
    MeshRefList *exclude_list = filter->exclude.entries[k];

    // cancel if mesh is actually excluded from the filter
    if (mesh_ref_list_find(exclude_list, mesh) != NULL)
      included = false;
  }

  return included;
}

/**
   Search if a mesh belong to a scene selection filter.
   Returns the target filter or NULL if not found.
 */
SceneSelectionFilter *
scene_selection_filter_find_mesh(SceneSelection *selection, Mesh *mesh) {

  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {
    SceneSelectionFilter *filter = &selection->filters[i];
    if (scene_selection_filter_include_mesh(filter, mesh))
      return filter;
  }

  return NULL;
}

void scene_selection_filter_add_mesh(SceneSelectionFilter *filter, Mesh *mesh) {
  mesh_ref_list_insert(&filter->selection, mesh);

  // transftert (optional)
  if (filter->transfert)
    mesh_ref_list_insert(filter->transfert, mesh);
}

/**
   Empty each filter's selection
 */
void scene_selection_empty(SceneSelection *selection) {

  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {
    SceneSelectionFilter *filter = &selection->filters[i];
    mesh_ref_list_empty(&filter->selection);

    if (filter->transfert)
      mesh_ref_list_empty(filter->transfert);
  }
}

/**
   Map Gizmo mode to mesh get attributes to apply correct transformation based
   in gizmo mode (trans/rot/scale).

   Used in the selection events when we need to cache the mesh attribute
   (loc/rot/scale) depending on the gizmo mode.
 */
static const mesh_get_transform_attribute mesh_transform_attribute[] = {
    [GizmoTransformMode_Translate] = mesh_get_position,
    [GizmoTransformMode_Rotate] = mesh_get_rotation_euler,
    [GizmoTransformMode_Scale] = mesh_get_scale,
};

/**
  Cache all meshes initial attribute based on gizmo mode (pos/rot/scale)
 */
void scene_selection_cache_initial_attributes(SceneSelection *selection,
                                              const GizmoTransformMode mode) {

  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++) {

    SceneSelectionFilter *filter = &selection->filters[i];

    for (size_t j = 0; j < filter->selection.length; j++) {
      Mesh *mesh = filter->selection.entries[j];
      vec3 attribute;
      mesh_transform_attribute[mode](mesh, &attribute);
      vec3_list_insert(&filter->init_attribute, attribute);
    }
  }
}

/**
  Empty all meshes initial attribute based on gizmo mode (pos/rot/scale)
 */
void scene_selection_empty_initial_attributes(SceneSelection *selection) {
  for (size_t i = 0; i < SCENE_SELECTION_TYPE_COUNT; i++)
    vec3_list_empty(&selection->filters[i].init_attribute);
}
