#include "key.h"
#include "../../../show.h"
#include "../core.h"
#include "../filter.h"
#include "../utils.h"
#include <stddef.h>
#include <stdint.h>

const uint8_t seq_count_select = 1;
const uint8_t seq_count_mode = 3;
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

static const uint8_t seq_count = 3;
/**
   List of hot key sequences.
   Note that the order is important. Switching entry 2 and 3 will break the
   overall sequence logic (1 hit G -> switch mode, 2 hit G -> transform).
 */
static const struct {
  SelectionKeySequence *sequences;
  size_t length;
} selection_key_sequences[3] = {
    {
        .sequences = selection_key_sequences_select,
        .length = seq_count_select,
    },
    {
        .sequences = selection_key_sequences_transform,
        .length = seq_count_transform,
    },
    {
        .sequences = selection_key_sequences_mode,
        .length = seq_count_mode,
    },
};

void scene_selection_init_key_events(Scene *scene) {

  for (size_t i = 0; i < seq_count; i++) {

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
  SceneSelection *selection = &scene->editor.selection;
  GizmoTransform *gizmo = &scene->editor.gizmo.transform;

  /* ==== DESELECT ALL ====   */
  // if already selection => unselect everything
  if (scene_selection_length(selection)) {
    // empty selection
    scene_selection_empty(selection);
    // hide gizmo
    scene_gizmo_transform_hide(scene);
  } else {
    /*  ==== SELECT ALL ==== */
    scene_selection_all(selection);

    // show gizmo
    scene_gizmo_transform_pos_to_selection(gizmo, &scene->editor.selection);
    scene_gizmo_transform_show(scene);
  }
}

void scene_selection_key_sequence_callback_set_gizmo_mode(
    KeyRecordSequence *seq, void *data) {

  Scene *scene = (Scene *)data;
  GizmoTransform *gizmo = &scene->editor.gizmo.transform;

  // hide gizmo
  scene_gizmo_transform_hide(scene);

  // search for same sequence in static array and assign mode to gizmo
  for (size_t i = 0; i < seq_count_mode; i++)
    if (keyrec_sequence_equal(selection_key_sequences_mode[i].sequence,
                              seq->sequence, seq->length)){
      gizmo->mode = selection_key_sequences_mode[i].mode;
      // reset hover colored on change mode
      gizmo_transform_reset_color_uniform(gizmo);
    }

  // show gizmo if has selection
  if (scene_selection_length(&scene->editor.selection)) {
    // update location to selection average
    scene_gizmo_transform_pos_to_selection(gizmo, &scene->editor.selection);
    scene_gizmo_transform_show(scene);
  }
}

void scene_selection_key_sequence_callback_transform(
    KeyRecordSequence *current_seq, void *data) {

  Scene *scene = (Scene *)data;
  GizmoTransform *gizmo = &scene->editor.gizmo.transform;
  MeshRefList *selection_list =
      scene_pipeline(scene, ScenePipeline_Fixed_Selection);

  // use the length as a flag to detect if gizmo already active or not
  if (scene_selection_length(&scene->editor.selection) == 0)
    return;

  // cache scene selection initial attributes
  scene_selection_empty_initial_attributes(&scene->editor.selection);

  for (size_t i = 0; i < seq_count_transform; i++) {

    SelectionKeySequence *key_seq = &selection_key_sequences_transform[i];
    // find equal key sequence
    if (keyrec_sequence_equal(current_seq->sequence, key_seq->sequence,
                              current_seq->length)) {

      Axis key_seq_axis = key_seq->axis;
      GizmoTransformMode key_seq_mode = key_seq->mode;

      // If gizmo is NOT already in the mode we do NOT transform
      // only switch mode
      if (key_seq_mode != gizmo->mode)
        return;

      // map axis from static sequences
      gizmo->axis = key_seq_axis;

      // cache scene selection initial attributes
      scene_selection_cache_initial_attributes(&scene->editor.selection,
                                               gizmo->mode);

      // set active handle from current mode and initialize offset
      gizmo_transform_set_active(gizmo, scene->active_camera, &scene->viewport);
    }
  }
}
