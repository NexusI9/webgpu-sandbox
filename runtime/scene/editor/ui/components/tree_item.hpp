#ifndef _SCENE_EDITOR_UI_COMPONENT_TREE_ITEM_H_
#define _SCENE_EDITOR_UI_COMPONENT_TREE_ITEM_H_

#include "imgui/imgui.h"
#include "runtime/scene/editor/ui/components/core.hpp"

namespace UI {

typedef enum {
  TreeItemFlag_None = 0,
  TreeItemFlag_AltBg = 1 << 0,
  TreeItemFlag_HasChild = 1 << 1,
} TreeItemFlag;

class TreeItemMesh : public Component {

public:
  TreeItemMesh(Scene *scene, const char *label, const reg_id_t id,
               const SceneEditorUIIcon icon, const int icon_size,
               const TreeItemFlag flag)
      : Component(scene, label), icon(icon), icon_size(icon_size), flag(flag),
        id(id) {}
  bool draw() override;
  bool draw_visibility();
  bool clicked;
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
  const SceneEditorUIIcon icon;
  const int icon_size;
  const TreeItemFlag flag;
  const reg_id_t id;
  ImVec2 pos;
  ImVec2 screen_pos;
};

class TreeItem : public Component {

public:
  TreeItem(Scene *scene, const char *label) : Component(scene, label) {}
  bool draw() override;
};

} // namespace UI

#endif
