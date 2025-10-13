#ifndef _SCENE_EDTIOR_UI_WINDOW_TREE_H_
#define _SCENE_EDTIOR_UI_WINDOW_TREE_H_

#include "backend/registry.h"
#include "core.hpp"
#include "runtime/scene/core.h"

namespace UI {

class TreeItem : public Window {

public:
  TreeItem(Scene *scene, const char *label, const reg_id_t id,
           const SceneEditorUIIcon icon, const int icon_size,
           const bool has_child)
      : Window(scene, label), icon(icon), icon_size(icon_size),
        has_child(has_child), id(id) {}
  void draw() override {};
  bool draw_label();
  bool draw_visibility();

private:
  const SceneEditorUIIcon icon;
  const int icon_size;
  const bool has_child;
  const reg_id_t id;
};

class Tree : public Window {

public:
  Tree(Scene *scene, const char *label) : Window(scene, label) {}
  void draw() override;

private:
  void draw_mesh(Mesh *);
  void draw_scene_editor_mesh_list(SceneEditorMeshList *, const RegEntryType);
};

} // namespace UI

#endif
