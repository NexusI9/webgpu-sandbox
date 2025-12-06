#include "highlight.h"
#include "utils/stli.h"

static inline GuiHighlightObject *gui_highlight_find(GuiHighlight *,
                                                     const void *);

GuiHighlightState gui_highlight_init(GuiHighlight *highlight) {

  highlight->count = 0;

  return GuiHighlightState_Success;
}

GuiHighlightState gui_highlight_listen(GuiHighlight *highlight) {

  for (size_t i = 0; i < highlight->count; i++) {

    GuiHighlightObject *object = &highlight->entries[i];

    if (object->hover_trigger(object->handle) && object->on_hover)
      object->on_hover(object->handle);
  }

  return GuiHighlightState_Success;
}

StaticListStatus gui_highlight_register(GuiHighlight *highlight,
                                        const GuiHighlightObject *object) {

  // prevent doublon
  if (gui_highlight_find(highlight, object->handle))
    return StaticListStatus_DuplicateEntry;

  return stli_insert(highlight->entries, GUI_HIGHLIGHT_CAPACITY,
                     &highlight->count, sizeof(GuiHighlightObject), object,
                     "GUI Highlight List");
}

StaticListStatus gui_highlight_unregister(GuiHighlight *highlight,
                                          const void *handle) {

  for (size_t i = 0; i < highlight->count; i++) {
    if (highlight->entries[i].handle == handle)
      return stli_remove_at_index(highlight->entries, &highlight->count,
                                  sizeof(GuiHighlightObject), i,
                                  "GUI Highlight List");
  }

  return StaticListStatus_UnfoundEntry;
}

GuiHighlightObject *gui_highlight_find(GuiHighlight *highlight,
                                       const void *handle) {
  for (size_t i = 0; i < highlight->count; i++)
    if (highlight->entries[i].handle == handle)
      return &highlight->entries[i];

  return NULL;
}

