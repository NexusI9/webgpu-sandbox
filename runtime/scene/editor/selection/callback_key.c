#include "callback_key.h"
#include "../../show.h"
#include "utils.h"
#include <stdint.h>

const uint8_t gizmo_mode_from_char[] = {
    ['G'] = GizmoTransformMode_Translate,
    ['S'] = GizmoTransformMode_Scale,
    ['R'] = GizmoTransformMode_Rotate,
};

void scene_selection_init_key_events(Scene *scene) {

  const uint8_t seq_count = 16;
  KeyRecordSequence sequences[16] = {
      // select all
      {
          .sequence = (key_t[]){'A'},
          .length = 1,
          .data = scene,
          .callback = scene_selection_key_sequence_callback_select_all,
          .owner = scene->id,
      },
      // mode switch
      {
          .sequence = (key_t[]){'G'},
          .length = 1,
          .data = scene,
          .callback = scene_selection_key_sequence_callback_set_gizmo_mode,
          .owner = scene->id,
      },
      {
          .sequence = (key_t[]){'S'},
          .length = 1,
          .data = scene,
          .callback = scene_selection_key_sequence_callback_set_gizmo_mode,
          .owner = scene->id,
      },
      {
          .sequence = (key_t[]){'R'},
          .length = 1,
          .data = scene,
          .callback = scene_selection_key_sequence_callback_set_gizmo_mode,
          .owner = scene->id,
      },
      // translate axis
      {
          .sequence = (key_t[]){'G'},
          .length = 2,
          .data = scene,
          .callback = scene_selection_key_sequence_callback_transform,
          .owner = scene->id,
      },
      {
          .sequence = (key_t[]){'G', 'X'},
          .length = 2,
          .data = scene,
          .callback = scene_selection_key_sequence_callback_transform,
          .owner = scene->id,
      },
      {
          .sequence = (key_t[]){'G', 'Y'},
          .length = 2,
          .data = scene,
          .callback = scene_selection_key_sequence_callback_transform,
          .owner = scene->id,
      },
      {
          .sequence = (key_t[]){'G', 'Z'},
          .length = 2,
          .data = scene,
          .callback = scene_selection_key_sequence_callback_transform,
          .owner = scene->id,
      },
      // scale axis
      {
          .sequence = (key_t[]){'S'},
          .length = 2,
          .data = scene,
          .callback = scene_selection_key_sequence_callback_transform,
          .owner = scene->id,
      },
      {
          .sequence = (key_t[]){'S', 'X'},
          .length = 2,
          .data = scene,
          .callback = scene_selection_key_sequence_callback_transform,
          .owner = scene->id,
      },
      {
          .sequence = (key_t[]){'S', 'Y'},
          .length = 2,
          .data = scene,
          .callback = scene_selection_key_sequence_callback_transform,
          .owner = scene->id,
      },
      {
          .sequence = (key_t[]){'S', 'Z'},
          .length = 2,
          .data = scene,
          .callback = scene_selection_key_sequence_callback_transform,
          .owner = scene->id,
      },
      // rotate axis
      {
          .sequence = (key_t[]){'R'},
          .length = 2,
          .data = scene,
          .callback = scene_selection_key_sequence_callback_transform,
          .owner = scene->id,
      },
      {
          .sequence = (key_t[]){'R', 'X'},
          .length = 2,
          .data = scene,
          .callback = scene_selection_key_sequence_callback_transform,
          .owner = scene->id,
      },
      {
          .sequence = (key_t[]){'R', 'Y'},
          .length = 2,
          .data = scene,
          .callback = scene_selection_key_sequence_callback_transform,
          .owner = scene->id,
      },
      {
          .sequence = (key_t[]){'R', 'Z'},
          .length = 2,
          .data = scene,
          .callback = scene_selection_key_sequence_callback_transform,
          .owner = scene->id,
      },
  };

  // dispatch to global input key record sequence
  for (size_t i = 0; i < seq_count; i++)
    input_key_sequence_add(&sequences[i]);
}

/**

▗▖ ▗▖▗▄▄▄▖▗▖  ▗▖▗▄▄▖  ▗▄▖  ▗▄▖ ▗▄▄▖ ▗▄▄▄
▐▌▗▞▘▐▌    ▝▚▞▘ ▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌▐▌  █
▐▛▚▖ ▐▛▀▀▘  ▐▌  ▐▛▀▚▖▐▌ ▐▌▐▛▀▜▌▐▛▀▚▖▐▌  █
▐▌ ▐▌▐▙▄▄▖  ▐▌  ▐▙▄▞▘▝▚▄▞▘▐▌ ▐▌▐▌ ▐▌▐▙▄▄▀

 ▗▄▄▖ ▗▄▖ ▗▖   ▗▖   ▗▄▄▖  ▗▄▖  ▗▄▄▖▗▖ ▗▖ ▗▄▄▖
▐▌   ▐▌ ▐▌▐▌   ▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌   ▐▌▗▞▘▐▌
▐▌   ▐▛▀▜▌▐▌   ▐▌   ▐▛▀▚▖▐▛▀▜▌▐▌   ▐▛▚▖  ▝▀▚▖
▝▚▄▄▖▐▌ ▐▌▐▙▄▄▖▐▙▄▄▖▐▙▄▞▘▐▌ ▐▌▝▚▄▄▖▐▌ ▐▌▗▄▄▞▘


 */
void scene_selection_key_sequence_callback_select_all(KeyRecordSequence *seq,
                                                      void *data) {

  Scene *scene = (Scene *)data;
  GizmoTransform *gizmo = &scene->editor.gizmo.transform;
  MeshRefList *selection_list =
      &scene->pipelines[ScenePipeline_Fixed_Selection];

  // if already selection => unselect everything
  if (selection_list->length) {
    // empty selection
    mesh_ref_list_empty(selection_list);
    // hide from the scene
    scene_hide_mesh_ref_list(scene, &gizmo->handles[gizmo->mode],
                             ScenePipeline_Fixed_Front);

  }
  // else select everything
  else {

    // Empty slection list first for safety
    mesh_ref_list_empty(selection_list);

    SceneLayer *exclude =
        scene_layer_set_find(&scene->layers, SCENE_LAYER_GIZMO_UNSELECTABLE);

    // Transfert all meshes from selectable pipeline
    // TODO: Unify the way we define the selectable pipelines
    mesh_ref_list_transfert(&scene->pipelines[ScenePipeline_Dynamic_Lit],
                            selection_list, &exclude->meshes);

    mesh_ref_list_transfert(&scene->pipelines[ScenePipeline_Dynamic_Unlit],
                            selection_list, &exclude->meshes);

    mesh_ref_list_transfert(&scene->pipelines[ScenePipeline_Dynamic_Lit],
                            selection_list, &exclude->meshes);

    // show gizmo
    scene_gizmo_transform_pos_to_selection(scene);
    scene_show_mesh_ref_list(scene, &gizmo->handles[gizmo->mode],
                             ScenePipeline_Fixed_Front);
  }
}

void scene_selection_key_sequence_callback_set_gizmo_mode(
    KeyRecordSequence *seq, void *data) {

  Scene *scene = (Scene *)data;
  GizmoTransform *gizmo = &scene->editor.gizmo.transform;
  MeshRefList *selection_list =
      &scene->pipelines[ScenePipeline_Fixed_Selection];

  // hide gizmo
  scene_gizmo_transform_hide(scene);

  // look up mode based on the sequence character (len = 1)
  for (size_t i = 0; i < seq->length; i++)
    gizmo->mode = gizmo_mode_from_char[seq->sequence[i]];

  // update location to selection average
  scene_gizmo_transform_pos_to_selection(scene);

  // show gizmo if has selection
  if (selection_list->length)
    scene_gizmo_transform_show(scene);
}

void scene_selection_key_sequence_callback_transform(KeyRecordSequence *seq,
                                                     void *data) {

  Scene *scene = (Scene *)data;
  GizmoTransform *gizmo = &scene->editor.gizmo.transform;
  MeshRefList *selection_list =
      &scene->pipelines[ScenePipeline_Fixed_Selection];

  // use the length as a flag to detect if gizmo already active or not
  if (selection_list->length == 0)
    return;

  printf("Transform gizmo\n");

  //gizmo_transform_set_active(gizmo, );
}
