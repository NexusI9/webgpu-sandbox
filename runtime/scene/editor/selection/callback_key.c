#include "callback_key.h"
#include "../../show.h"
#include "utils.h"
#include <stddef.h>
#include <stdint.h>

const uint8_t gizmo_mode_from_char[] = {
    ['G'] = GizmoTransformMode_Translate,
    ['S'] = GizmoTransformMode_Scale,
    ['R'] = GizmoTransformMode_Rotate,
};

const uint8_t seq_count_select = 1;
const uint8_t seq_count_mode = 3;
const uint8_t seq_count_transform = 12;
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
    },
    {
        .sequence = {'S'},
        .length = 1,
        .callback = scene_selection_key_sequence_callback_set_gizmo_mode,
    },
    {
        .sequence = {'R'},
        .length = 1,
        .callback = scene_selection_key_sequence_callback_set_gizmo_mode,
    },
};

static SelectionKeySequence selection_key_sequences_transform[12] = {
    // translate axis
    {
        .sequence = {'G'},
        .length = 1,
        .callback = scene_selection_key_sequence_callback_transform,
        .axis = Axis_View,
    },
    {
        .sequence = {'G', 'X'},
        .length = 2,
        .callback = scene_selection_key_sequence_callback_transform,
        .axis = Axis_X,
    },
    {
        .sequence = {'G', 'Y'},
        .length = 2,
        .callback = scene_selection_key_sequence_callback_transform,
        .axis = Axis_Y,
    },
    {
        .sequence = {'G', 'Z'},
        .length = 2,
        .callback = scene_selection_key_sequence_callback_transform,
        .axis = Axis_Z,
    },
    // scale axis
    {
        .sequence = {'S'},
        .length = 1,
        .callback = scene_selection_key_sequence_callback_transform,
        .axis = Axis_XYZ,
    },
    {
        .sequence = {'S', 'X'},
        .length = 2,
        .callback = scene_selection_key_sequence_callback_transform,
        .axis = Axis_X,
    },
    {
        .sequence = {'S', 'Y'},
        .length = 2,
        .callback = scene_selection_key_sequence_callback_transform,
        .axis = Axis_Y,
    },
    {
        .sequence = {'S', 'Z'},
        .length = 2,
        .callback = scene_selection_key_sequence_callback_transform,
        .axis = Axis_Z,
    },
    // rotate axis
    {
        .sequence = {'R'},
        .length = 1,
        .callback = scene_selection_key_sequence_callback_transform,
        .axis = Axis_View,
    },
    {
        .sequence = {'R', 'X'},
        .length = 2,
        .callback = scene_selection_key_sequence_callback_transform,
        .axis = Axis_X,
    },
    {
        .sequence = {'R', 'Y'},
        .length = 2,
        .callback = scene_selection_key_sequence_callback_transform,
        .axis = Axis_Y,
    },
    {
        .sequence = {'R', 'Z'},
        .length = 2,
        .callback = scene_selection_key_sequence_callback_transform,
        .axis = Axis_Z,
    },
};

static const struct {
  SelectionKeySequence *sequences;
  size_t length;
} selection_key_sequences[3] = {
    {
        .sequences = selection_key_sequences_select,
        .length = seq_count_select,
    },
    {
        .sequences = selection_key_sequences_mode,
        .length = seq_count_mode,
    },
    {
        .sequences = selection_key_sequences_transform,
        .length = seq_count_transform,
    },
};

void scene_selection_init_key_events(Scene *scene) {

  for (size_t i = 0; i < 3; i++) {

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

  // look up mode based on the sequence character (len = 1)
  for (size_t i = 0; i < seq->length; i++)
    gizmo->mode = gizmo_mode_from_char[seq->sequence[i]];

  // update location to selection average
  scene_gizmo_transform_pos_to_selection(scene);

  // show gizmo if has selection
  if (selection_list->length)
    scene_gizmo_transform_show(scene);
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

  // map axis from static sequences
  vec3 axis;
  for (size_t i = 0; i < seq_count_transform; i++)
    if (keyrec_sequence_equal(current_seq->sequence,
                              selection_key_sequences_transform[i].sequence,
                              current_seq->length)) {
      vec_world_axis(selection_key_sequences_transform[i].axis, &axis);
      break;
    }

  // set active handle from current mode and initialize offset
  gizmo_transform_set_active(gizmo, axis,
                             &scene->pipelines[ScenePipeline_Fixed_Selection],
                             scene->active_camera, &scene->viewport);
}
