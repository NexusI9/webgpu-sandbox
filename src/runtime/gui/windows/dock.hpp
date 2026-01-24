#ifndef _WINDOW_DOCKED_H_
#define _WINDOW_DOCKED_H_

#include "runtime/gui/utils.hpp"
#include <imgui/imgui.h>

namespace UI {

class DockedWindow {

public:
  DockedWindow(){};

  void Begin(const char *label, ImVec2 position, ImVec2 size,
             const ImGuiWindowFlags flags = 0) {

    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin(label, NULL, DOCK_WINDOW_FLAGS | flags);
  }

  void End() {
    ImGui::End();
    ImGui::PopStyleVar(2);
  }
};

} // namespace UI

#endif
