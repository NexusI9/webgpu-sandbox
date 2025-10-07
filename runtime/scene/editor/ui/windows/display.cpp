#include "display.hpp"

#include "../imgui_style/style.carbon.hpp"
#include "runtime/scene/editor/ui/components/ButtonIcon.hpp"
#include "runtime/scene/show.h"

void UI::Display::draw() {

  ImGui::SetNextWindowPos(ImVec2(ui->size[SceneEditorUISize_Gizmo_Margin], 0),
                          ImGuiCond_Always);
  ImGui::Begin("Display Frame", nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoBackground);

  // remove backgrounds & padding
  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
  ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
  {

    {
      bool layout_button =
          ButtonIcon(ui, SceneEditorUIIcon_Layout, "Layout",
                     ImVec2(ui->size[SceneEditorUISize_Button_DisplaySize],
                            ui->size[SceneEditorUISize_Button_DisplaySize]))
              .draw();

      if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Toggle interface");

      if (layout_button)
        state ^= UIDisplay_Layout;
    }

    ImGui::SameLine();

    {
      bool activity_button =
          ButtonIcon(ui, SceneEditorUIIcon_Activity, "Activity",
                     ImVec2(ui->size[SceneEditorUISize_Button_DisplaySize],
                            ui->size[SceneEditorUISize_Button_DisplaySize]))
              .draw();

      if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Toggle monitor");

      if (activity_button)
        state ^= UIDisplay_Activity;
    }
  }
  ImGui::PopStyleColor(3);
  ImGui::PopStyleVar();
  ImGui::End();
}
