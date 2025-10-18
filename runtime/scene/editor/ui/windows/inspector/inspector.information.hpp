#ifndef _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_INFORMATION_H_
#define _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_INFORMATION_H_

#include "runtime/scene/editor/ui/windows/display.hpp"
#include "runtime/scene/editor/ui/windows/inspector/tab.hpp"

namespace UI {

class InfoTab : public InspectorTab {

public:
  InfoTab(Scene *scene, const SceneEditorUIIcon icon, const char *label)
      : InspectorTab(scene, icon, label) {}
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
