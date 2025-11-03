#include "browser.hpp"
#include "imgui/imgui_impl_wgpu.h"

void UI::Browser::draw() {

  ImGui::BeginChild("##Browser", ImVec2(0, 0), true);
  {
    ImGui::Text("%s", label);
  }
  ImGui::EndChild();
}
