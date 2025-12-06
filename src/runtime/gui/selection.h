#ifndef _EMMA_SELECTION_H_
#define _EMMA_SELECTION_H_

#include <stdbool.h>

typedef enum {
  // No click emitted during the session
  GuiSelectionStatus_Off,

  // Click was emitted but no hit
  GuiSelectionStatus_Blank,

  // Click was emitted and hit
  GuiSelectionStatus_Hit,
} GuiSelectionStatus;

typedef struct {

  GuiSelectionStatus status;
  int click_count;
  int hit_count;

} GuiSelection;

static inline void gui_selection_init(GuiSelection *selection) {
  selection->click_count = 0;
  selection->hit_count = 0;
  selection->status = GuiSelectionStatus_Off;
}

static inline bool gui_selection_blank(GuiSelection *selection, bool trigger) {

  // we don't reset back to blank if already hit during the session.
  if (trigger && selection->status != GuiSelectionStatus_Hit) {
    selection->click_count++;
    selection->status = GuiSelectionStatus_Blank;
  }

  return trigger;
}

static inline bool gui_selection_hit(GuiSelection *selection, bool trigger) {

  if (trigger) {
    selection->click_count++;
    selection->hit_count++;
    selection->status = GuiSelectionStatus_Hit;
  }

  return trigger;
}

static inline bool gui_selection_begin(GuiSelection *selection, bool trigger) {

  return gui_selection_blank(selection, trigger);
}

static inline GuiSelectionStatus selection_end(GuiSelection *selection) {

  if (selection->status == GuiSelectionStatus_Blank) {
    selection->click_count = 0;
    selection->hit_count = 0;
  }

  selection->status = GuiSelectionStatus_Off;

  return selection->status;
}

static inline GuiSelectionStatus selection_status(GuiSelection *selection) {
  return selection->status;
}

#endif
