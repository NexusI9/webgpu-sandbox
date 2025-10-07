#ifndef _SCENE_EDTIOR_UI_WINDOW_CORE_H_
#define _SCENE_EDTIOR_UI_WINDOW_CORE_H_

#include "runtime/scene/core.h"
#include "runtime/scene/editor/ui/core.h"

namespace UI {

class Window {

public:
  Window(Scene *scene, const char *label)
      : scene(scene), ui(&scene->editor.ui), label(label) {}
  virtual void draw() = 0;

protected:
  const char *label;
  Scene *scene;
  SceneEditorUI *ui;
};

} // namespace UI

#endif
