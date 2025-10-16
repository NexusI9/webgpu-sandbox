#ifndef _SCENE_EDTIOR_UI_WINDOW_TREE_H_
#define _SCENE_EDTIOR_UI_WINDOW_TREE_H_

#include "backend/registry.h"
#include "core.hpp"
#include "runtime/scene/core.h"

namespace UI {

class Tree : public Window {

public:
  Tree(Scene *scene, const char *label) : Window(scene, label) {}
  void draw() override;

private:
  void draw_mesh(Mesh *, const size_t);
  void draw_scene_editor_mesh_list(SceneEditorMeshList *, const RegEntryType,
                                   const size_t);
};

} // namespace UI

#endif
