#ifndef _GUI_COMPONENTS_BUTTON_GROUP_H_
#define _GUI_COMPONENTS_BUTTON_GROUP_H_

#include "imgui/imgui.h"
#include "runtime/gui/components/core.hpp"
#include "runtime/gui/core.h"
#include "runtime/scene/core.h"

namespace UI {

static const int BUTTON_GROUP_DEFAULT_SELECTED_NONE = -1;

typedef enum {
  ButtonGroupDirection_Vertical,
  ButtonGroupDirection_Horizontal,
} ButtonGroupDirection;

typedef struct {
  const char *label;
  ThemeIcon icon;
  void (*on_click_callback)(Scene *, Renderer *, void *);
  void *user_data;
  const char *tooltip;
} ButtonGroupItem;

typedef struct {
  const ImVec4 background_default, background_hover, background_active;
  const float border_radius;
  const int size;
  const float inner_padding;
  const int x, y;
} ButtonStyle;

class ButtonGroup : public Component {

public:
  ButtonGroup(
      Gui *gui, const char *label, const ButtonGroupItem *items,
      const uint8_t items_length, const ButtonStyle *style,
      const uint8_t default_selected = BUTTON_GROUP_DEFAULT_SELECTED_NONE,
      const ButtonGroupDirection direction = ButtonGroupDirection_Vertical)
      : Component(gui, label), items(items), items_length(items_length),
        style(style), direction(direction), selected(default_selected) {}
  bool draw() override;

private:
  uint8_t selected;
  const ButtonGroupItem *items;
  const uint8_t items_length;
  const ButtonStyle *style;
  const ButtonGroupDirection direction;
};

} // namespace UI

#endif
