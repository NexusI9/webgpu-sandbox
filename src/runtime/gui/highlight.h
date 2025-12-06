#ifndef _GUI_HIGHLIGHT_H_
#define _GUI_HIGHLIGHT_H_

#include "utils/defines.h"
#include "utils/stli.h"
#include <cglm/cglm.h>
#include <stddef.h>

static const int GUI_HIGHLIGHT_CAPACITY = 1024;

typedef enum {
  GuiHighlightState_Success,
  GuiHighlightState_MaxCapacity,
  GuiHighlightState_UndefError,
} GuiHighlightState;

typedef bool (*gui_highlight_hover_trigger)(void *);
typedef void (*gui_highlight_on_hover)(void *);
typedef void (*gui_highlight_on_leave)(void *);

typedef struct {
  void *handle;
  gui_highlight_hover_trigger hover_trigger;
  gui_highlight_on_hover on_hover;
  gui_highlight_on_leave on_leave;
} GuiHighlightObject;

typedef struct {

  GuiHighlightObject entries[GUI_HIGHLIGHT_CAPACITY];
  size_t count;

} GuiHighlight;

EXTERN_C_BEGIN

// Retained Mode
GuiHighlightState gui_highlight_init(GuiHighlight *);

GuiHighlightState gui_highlight_listen(GuiHighlight *);

StaticListStatus gui_highlight_register(GuiHighlight *,
                                        const GuiHighlightObject *);

StaticListStatus gui_highlight_unregister(GuiHighlight *, const void *);

// Immediate mode
static inline GuiHighlightState
gui_highlight_listen_im(const GuiHighlightObject *object) {

  if (object->hover_trigger(object->handle) && object->on_hover)
    object->on_hover(object->handle);

  return GuiHighlightState_Success;
}


EXTERN_C_END

#endif
