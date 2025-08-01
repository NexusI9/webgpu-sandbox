#include "callback_key.h"
#include "../../show.h"
#include "utils.h"
#include <stddef.h>
#include <stdint.h>

const uint8_t seq_count_select = 1;
const uint8_t seq_count_mode = 0;
const uint8_t seq_count_transform = 6;
const uint8_t seq_count_total =
    seq_count_select + seq_count_mode + seq_count_transform;

static SelectionKeySequence selection_key_sequences_select[1] = {
    // select all
    {
        .sequence = {'A'},
        .length = 1,
        .callback = scene_selection_key_sequence_callback_select_all,
    },
};

static SelectionKeySequence selection_key_sequences_mode[3] = {
    // mode switch
    {
        .sequence = {'G'},
        .length = 1,
        .callback = scene_selection_key_sequence_callback_set_gizmo_mode,
        .mode = GizmoTransformMode_Translate,
    },
    {
        .sequence = {'S'},
        .length = 1,
        .callback = scene_selection_key_sequence_callback_set_gizmo_mode,
        .mode = GizmoTransformMode_Scale,
    },
    {
        .sequence = {'R'},
        .length = 1,
        .callback = scene_selection_key_sequence_callback_set_gizmo_mode,
        .mode = GizmoTransformMode_Rotate,
    },
};

static SelectionKeySequence selection_key_sequences_transform[6] = {
    // transform view/general
    {
        .sequence = {'G'},
        .length = 1,
        .callback = scene_selection_key_sequence_callback_transform,
        .axis = Axis_View,
        .mode = GizmoTransformMode_Translate,
    },
    {
        .sequence = {'R'},
        .length = 1,
        .callback = scene_selection_key_sequence_callback_transform,
        .axis = Axis_View,
        .mode = GizmoTransformMode_Rotate,
    },
    {
        .sequence = {'S'},
        .length = 1,
        .callback = scene_selection_key_sequence_callback_transform,
        .axis = Axis_XYZ,
        .mode = GizmoTransformMode_Scale,
    },
    // transform axis
    {
        .sequence = {'X'},
        .length = 1,
        .callback = scene_selection_key_sequence_callback_transform,
        .axis = Axis_X,
    },
    {
        .sequence = {'Y'},
        .length = 1,
        .callback = scene_selection_key_sequence_callback_transform,
        .axis = Axis_Y,
    },
    {
        .sequence = {'Z'},
        .length = 1,
        .callback = scene_selection_key_sequence_callback_transform,
        .axis = Axis_Z,
    },
};

static const struct {
  SelectionKeySequence *sequences;
  size_t length;
} selection_key_sequences[2] = {
    {
        .sequences = selection_key_sequences_select,
        .length = seq_count_select,
    },
    /*
    {
        .sequences = selection_key_sequences_mode,
        .length = seq_count_mode,
    },
     */
    {
        .sequences = selection_key_sequences_transform,
        .length = seq_count_transform,
    },
};

void scene_selection_init_key_events(Scene *scene) {

  for (size_t i = 0; i < 2; i++) {

    // dispatch to global input key record sequence
    SelectionKeySequence *sequences = selection_key_sequences[i].sequences;
    size_t count = selection_key_sequences[i].length;

    for (size_t j = 0; j < count; j++) {
      SelectionKeySequence *seq = &sequences[j];
      input_key_sequence_add(&(KeyRecordSequence){
          .sequence = seq->sequence,
          .callback = seq->callback,
          .length = seq->length,
          .data = scene,
          .owner = scene->id,
      });
    }
  }
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

  // search for same sequence in static array and assign mode to gizmo
  for (size_t i = 0; i < seq_count_mode; i++)
    if (keyrec_sequence_equal(selection_key_sequences_mode[i].sequence,
                              seq->sequence, seq->length))
      gizmo->mode = selection_key_sequences_mode[i].mode;

  // show gizmo if has selection
  if (selection_list->length) {
    // update location to selection average
    scene_gizmo_transform_pos_to_selection(scene);
    scene_gizmo_transform_show(scene);
  }
}

void scene_selection_key_sequence_callback_transform(
    KeyRecordSequence *current_seq, void *data) {

  Scene *scene = (Scene *)data;
  GizmoTransform *gizmo = &scene->editor.gizmo.transform;
  MeshRefList *selection_list =
      &scene->pipelines[ScenePipeline_Fixed_Selection];

  // use the length as a flag to detect if gizmo already active or not
  if (selection_list->length == 0)
    return;

  for (size_t i = 0; i < seq_count_transform; i++) {

    SelectionKeySequence *key_seq = &selection_key_sequences_transform[i];
    // find equal key sequence
    if (keyrec_sequence_equal(current_seq->sequence, key_seq->sequence,
                              current_seq->length)) {

      Axis key_seq_axis = key_seq->axis;
      GizmoTransformMode key_seq_mode = key_seq->mode;

      // If gizmo is NOT already in the mode we do NOT transform
      // only switch mode
      if (key_seq_mode != gizmo->mode) {

        scene_gizmo_transform_hide(scene);

        gizmo->mode = key_seq_mode;

        // update location to selection average and redisplay it
        scene_gizmo_transform_pos_to_selection(scene);
        scene_gizmo_transform_show(scene);
        return;
      }

      // map axis from static sequences
      gizmo->axis = key_seq_axis;

      // set gizmo position to center of selection
      scene_gizmo_transform_pos_to_selection(scene);

      // set active handle from current mode and initialize offset
      gizmo_transform_set_active(
          gizmo, &scene->pipelines[ScenePipeline_Fixed_Selection],
          scene->active_camera, &scene->viewport);
    }
  }
}
