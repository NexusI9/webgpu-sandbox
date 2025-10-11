#ifndef _SCENE_EDITOR_UI_COMPONENTS_BUTTON_GROUP_H_
#define _SCENE_EDITOR_UI_COMPONENTS_BUTTON_GROUP_H_

#include "imgui/imgui.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/ui/core.h"
#include "runtime/scene/editor/ui/windows/core.hpp"

namespace UI {

typedef struct {
  const char *label;
  SceneEditorUIIcon icon;
  void (*on_click_callback)(Scene *);
} ButtonGroupItem;

typedef struct {
  const ImVec4 background_default, background_hover, background_active;
  const float border_radius;
  const int size;
  const float inner_padding;
  const int x, y;
} ButtonStyle;

class ButtonGroup : public Window {

public:
  ButtonGroup(Scene *scene, const char *label, const ButtonGroupItem *items,
              const uint8_t items_length, const ButtonStyle *style)
      : Window(scene, label), items(items), items_length(items_length),
        style(style) {}
  void draw() override;

private:
  const ButtonGroupItem *items;
  const uint8_t items_length;
  const ButtonStyle *style;
};

} // namespace UI

#endif
