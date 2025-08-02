#include "core.h"
#include "../../show.h"
#include "./callback_key.h"
#include "./callback_mouse.h"
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
  GizmoTransform *gizmo = &cast_scene->editor.gizmo.transform;
  MeshRefList *selection_list = &gizmo->cache.selection;
  // look-up transform callback
  gizmo_transform_callback transform_callback =
      gizmo->transform_callback[gizmo->mode];

  if (selection_list->length)
    transform_callback(gizmo, cast_scene->active_camera, &cast_scene->viewport);
}

/**
   Define selection rules for each selection lists (mesh or shader-based
   highlight). Configure the included and exclude the mesh reference lists from
   the scene to know which objects can be selected.

   Note that both selection lists mutually exclude each others so they do not
   interfere.
 */
void scene_selection_init_rules(Scene *scene) {

  SceneLayer *exclude_layer =
      scene_layer_set_find(&scene->layers, SCENE_LAYER_UNSELECTABLE);

  /*

    ====  mesh based selection ====

   */

  SceneSelectionSet *mesh_rules = &scene->editor.selection[SceneSelectionType_Mesh];

  // include
  mesh_rules->include.entries[0] = &scene->pipelines[ScenePipeline_Dynamic_Lit];
  mesh_rules->include.entries[1] =
      &scene->pipelines[ScenePipeline_Dynamic_Unlit];
  mesh_rules->include.length = 2;

  // exclude
  mesh_rules->exclude.entries[0] = &exclude_layer->meshes;
  mesh_rules->exclude.entries[1] = &scene->pipelines[ScenePipeline_Fixed];
  mesh_rules->exclude.length = 2;

  // create source list
  mesh_ref_list_create(&mesh_rules->source, MESH_REF_LIST_CAPACITY);

  // define destination (i.e. the pipeline where the selected meshes will be
  // pushes to)
  mesh_rules->destination = &scene->pipelines[ScenePipeline_Fixed_Selection];

  /*

    ==== shader based selection ====

   */

  SceneSelectionSet *shader_rules = &scene->editor.selection[SceneSelectionType_Shader];

  // include
  shader_rules->include.entries[0] = &scene->pipelines[ScenePipeline_Fixed];
  shader_rules->include.length = 1;

  // exclude
  shader_rules->exclude.entries[0] = &exclude_layer->meshes;
  shader_rules->exclude.entries[1] = mesh_rules->include.entries[0];
  shader_rules->exclude.entries[2] = mesh_rules->include.entries[1];
  shader_rules->exclude.length = 3;

  // create source list
  mesh_ref_list_create(&shader_rules->source, MESH_REF_LIST_CAPACITY);

  // set destination to NULL (no need to add selected meshes to a specific
  // pipeline)
  shader_rules->destination = NULL;
}

