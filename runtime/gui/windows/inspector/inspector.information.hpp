#ifndef _GUI_WINDOW_INSPECTOR_INFORMATION_H_
#define _GUI_WINDOW_INSPECTOR_INFORMATION_H_

#include "backend/theme/core.h"
#include "runtime/gui/windows/display.hpp"
#include "runtime/gui/windows/inspector/tab.hpp"

namespace UI {

class InfoTab : public InspectorTab {

public:
  InfoTab(Gui* gui, const ThemeIcon icon, const char *label)
      : InspectorTab(gui, icon, label) {}
  void draw() override;
  static int state;

private:
  static constexpr const char *backend_label[] = {
      "Undefined", "Null",   "WebGPU", "D3D11",    "D3D12",
      "Metal",     "Vulkan", "OpenGL", "OpenGLES", "Force32",
  };

  static constexpr const char *adapter_type_label[] = {
      "Discrete GPU", "Integrated GPU", "CPU", "Unknown", "Force32",
  };
};

} // namespace UI

#endif
