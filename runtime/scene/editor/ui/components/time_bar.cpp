#include "time_bar.hpp"
#include "imgui/imgui.h"

void UI::TimeBar::draw() {

  ImDrawList *draw_list = ImGui::GetWindowDrawList();

  ImVec2 pos = ImGui::GetCursorScreenPos();
  ImVec2 surface = ImGui::GetContentRegionAvail();

  // background
  draw_list->AddRectFilled(pos, ImVec2(surface.x, style->height),
                           ImGui::ColorConvertFloat4ToU32(style->background),
                           style->border_radius);

  // bar
  double x = value * surface.x / max_value;
  draw_list->AddRectFilled(pos, ImVec2(x, style->height),
                           ImGui::ColorConvertFloat4ToU32(style->bar),
                           style->border_radius);

  ImGui::Text("%s", label);
  ImGui::SameLine();
  ImGui::SetCursorPosX(surface.x - 220);
  ImGui::Text("%.2fms", value);
}
