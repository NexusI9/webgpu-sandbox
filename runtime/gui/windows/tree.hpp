#ifndef _GUI_WINDOW_TREE_H_
#define _GUI_WINDOW_TREE_H_

#include "backend/registry.h"
#include "core.hpp"
#include "runtime/scene/core.h"

namespace UI {

class Tree : public Window {

public:
  Tree(Gui *gui, const char *label) : Window(gui, label) {}
  void draw() override;

private:
  void draw_mesh(Mesh *, const size_t);
  void draw_mesh_list(SceneEditorMeshList *, const RegEntryType, const size_t);
};

} // namespace UI

#endif
