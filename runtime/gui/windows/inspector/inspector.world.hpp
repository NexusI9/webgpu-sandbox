#ifndef _GUI_WINDOW_INSPECTOR_WORLD_H_
#define _GUI_WINDOW_INSPECTOR_WORLD_H_

#include "runtime/gui/components/combobox_resolution.hpp"
#include "runtime/gui/windows/inspector/tab.hpp"
#include "runtime/texture/core.h"

namespace UI {

class WorldTab : public InspectorTab {

public:
  WorldTab(Gui* gui, const ThemeIcon icon, const char *label)
      : InspectorTab(gui, icon, label) {}
  void draw() override;

private:
  static void on_resolution_change_point_light(Scene*, const TextureResolution);
  static void on_resolution_change_dir_light(Scene*, const TextureResolution);
  static void on_resolution_change_plane_reflection(Scene*, const TextureResolution);
};

} // namespace UI

#endif
