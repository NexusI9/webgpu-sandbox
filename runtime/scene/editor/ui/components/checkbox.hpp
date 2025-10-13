#ifndef _SCENE_EDITOR_UI_CHECKBOX_H_
#define _SCENE_EDITOR_UI_CHECKBOX_H_

#include "runtime/scene/editor/ui/core.h"
#include "runtime/scene/editor/ui/windows/core.hpp"
namespace UI {

class Checkbox : public Window {

public:
  Checkbox(Scene *scene, const char *label, bool active,
           const SceneEditorUIIcon icon,
           void (*on_change)(Scene *, bool, void *), void *user_data)
      : Window(scene, label), icon(icon), on_change(on_change),
        user_data(user_data), active(active) {}

  void draw() override;

private:
  const SceneEditorUIIcon icon;
  void (*on_change)(Scene *, bool, void *);
  void *user_data;
  bool active;
};

} // namespace UI

#endif
