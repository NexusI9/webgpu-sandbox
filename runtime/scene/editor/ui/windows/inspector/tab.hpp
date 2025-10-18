#ifndef _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_TAB_H_
#define _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_TAB_H_

#include "runtime/scene/editor/ui/windows/core.hpp"

namespace UI {

class InspectorTab : public Window {

public:
  InspectorTab(Scene *scene, const SceneEditorUIIcon icon, const char *label)
      : Window(scene, label), icon(icon), tooltip(label) {}
  virtual void draw() = 0;
  const SceneEditorUIIcon icon;
  const char *tooltip;
};

} // namespace UI

#endif
