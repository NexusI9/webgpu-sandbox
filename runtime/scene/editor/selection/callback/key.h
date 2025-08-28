#ifndef _SCENE_EDITOR_CALLBACK_KEY_H_
#define _SCENE_EDITOR_CALLBACK_KEY_H_

#include "../../../core.h"
#include "../runtime/input/input.h"

typedef struct {
  key_t sequence[3];
  size_t length;
  input_keyrec_callback callback;
  Axis axis;
  GizmoMode mode;
} SelectionKeySequence;

void scene_selection_init_key_events(Scene *scene);

// key event callback
void scene_selection_key_sequence_callback_select_all(KeyRecordSequence *,
                                                      void *);

void scene_selection_key_sequence_callback_set_gizmo_mode(KeyRecordSequence *,
                                                          void *);

void scene_selection_key_sequence_callback_transform(KeyRecordSequence *,
                                                     void *);

#endif
