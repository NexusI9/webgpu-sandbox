#ifndef _GUI_COMPONENT_TREE_ITEM_H_
#define _GUI_COMPONENT_TREE_ITEM_H_

#include "imgui/imgui.h"
#include "runtime/gui/components/core.hpp"

namespace UI {

typedef enum {
  TreeItemFlag_None = 0,
  TreeItemFlag_AltBg = 1 << 0,
  TreeItemFlag_HasChild = 1 << 1,
} TreeItemFlag;

class TreeItemMesh : public Component {

public:
  TreeItemMesh(Gui *gui, const char *label, const reg_id_t id,
               const ThemeIcon icon, const int icon_size, const TreeItemFlag flag)
      : Component(gui, label), icon(icon), icon_size(icon_size), flag(flag),
        id(id) {}
  bool draw() override;
  bool draw_visibility();
  bool clicked = false;
  void close_click() { clicked = false; }
  /*
    Need to be called at the end of the click condition (similar to TreePop)
    to make sure the state doesn't trail during the whole loop.

    Example:

    if(tree_item.is_clicked){
       ....
       tree_item.close_click();
    }

  */

private:
  static constexpr int height = 25;
  static constexpr int border_radius = 2;
  static constexpr int top_padding = 3;
  static constexpr int left_padding = 20;
  const ThemeIcon icon;
  const int icon_size;
  const TreeItemFlag flag;
  const reg_id_t id;
  ImVec2 pos;
  ImVec2 screen_pos;
};

class TreeItem : public Component {

public:
  TreeItem(Gui *gui, const char *label) : Component(gui, label) {}
  bool draw() override;
};

} // namespace UI

#endif
