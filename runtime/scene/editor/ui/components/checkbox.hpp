#ifndef _SCENE_EDITOR_UI_CHECKBOX_H_
#define _SCENE_EDITOR_UI_CHECKBOX_H_

#include "runtime/scene/editor/ui/components/core.hpp"
#include "runtime/scene/editor/ui/core.h"

namespace UI {

class Checkbox : public Component {

public:
  Checkbox(Scene *scene, const char *label, bool active,
           const SceneEditorUIIcon icon,
           void (*on_change)(Scene *, bool, void *), void *user_data)
      : Component(scene, label), icon(icon), on_change(on_change),
        user_data(user_data), active(active) {}

  bool draw() override;

private:
  const SceneEditorUIIcon icon;
  void (*on_change)(Scene *, bool, void *);
  void *user_data;
  bool active;
};

} // namespace UI

#endif
