#ifndef _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_WORLD_H_
#define _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_WORLD_H_

#include "runtime/scene/editor/ui/components/combobox_resolution.hpp"
#include "runtime/scene/editor/ui/windows/inspector/tab.hpp"
#include "runtime/texture/core.h"

namespace UI {

class WorldTab : public InspectorTab {

public:
  WorldTab(Scene *scene, const SceneEditorUIIcon icon, const char *label)
      : InspectorTab(scene, icon, label) {}
  void draw() override;

private:
  static void on_resolution_change_point_light(Scene*, const TextureResolution);
  static void on_resolution_change_dir_light(Scene*, const TextureResolution);
  static void on_resolution_change_plane_reflection(Scene*, const TextureResolution);
};

} // namespace UI

#endif
