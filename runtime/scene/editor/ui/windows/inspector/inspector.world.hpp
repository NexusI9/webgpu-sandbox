#ifndef _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_WORLD_H_
#define _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_WORLD_H_

#include "runtime/scene/editor/ui/windows/inspector/tab.hpp"

namespace UI {

class WorldTab : public InspectorTab {

public:
  WorldTab(Scene *scene, const SceneEditorUIIcon icon, const char *label)
      : InspectorTab(scene, icon, label) {}
  void draw() override;
};

} // namespace UI

#endif
