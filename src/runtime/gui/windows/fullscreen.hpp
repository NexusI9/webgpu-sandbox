#ifndef _WINDOW_FULLSCREEN_H_
#define _WINDOW_FULLSCREEN_H_

#include <imgui/imgui.h>

namespace UI {

class FullScreenWindow {

public:
  FullScreenWindow(){};
  void Begin(const char *label) {
    ImGuiViewport *vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->Pos);
    ImGui::SetNextWindowSize(vp->Size);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin(label, NULL,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse |
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoBringToFrontOnFocus);
  }

  void End() {
    ImGui::End();
    ImGui::PopStyleVar(2);
  }
};

} // namespace UI

#endif
