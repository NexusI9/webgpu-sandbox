#include "gizmo.hpp"
#include "imgui/imgui.h"
#include "runtime/scene/editor/selection/core.h"
#include "runtime/scene/editor/selection/utils.h"
#include "runtime/scene/editor/ui/components/ButtonIcon.hpp"

static const struct {
  const GizmoMode mode;
  const char *label;
  const SceneEditorUIIcon icon;
} gizmo_button[] = {
    {GizmoMode_Position, "Position", SceneEditorUIIcon_Gizmo_Position},
    {GizmoMode_Rotation, "Rotate", SceneEditorUIIcon_Gizmo_Rotate},
    {GizmoMode_Scale, "Scale", SceneEditorUIIcon_Gizmo_Scale},
};

void UI::Gizmo::draw() {

  ImGui::SetNextWindowPos(
      ImVec2(ui->size[SceneEditorUISize_Gizmo_Margin],
             (int)(ui->size[SceneEditorUISize_Screen_Height] / 2) -
                 (int)(ui->size[SceneEditorUISize_Gizmo_Height] / 2)));

  ImGui::SetNextWindowSize(ImVec2(ui->size[SceneEditorUISize_Gizmo_Width],
                                  ui->size[SceneEditorUISize_Gizmo_Height]));
  ImGui::Begin(label, nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_NoBackground);

  for (uint8_t i = 0; i < 3; i++) {
    if (ButtonIcon(ui, gizmo_button[i].icon, gizmo_button[i].label,
                   ImVec2(ui->size[SceneEditorUISize_Button_GizmoSize],
                          ui->size[SceneEditorUISize_Button_GizmoSize]))
            .draw()) {
      scene_gizmo_hide(scene);
      scene->editor.gizmo.transform.mode = gizmo_button[i].mode;
      if (scene_selection_length(&scene->editor.selection)) {
        scene_gizmo_pos_to_selection(&scene->editor.gizmo.transform,
                                     &scene->editor.selection,
                                     &scene->renderer.ssbo);
        scene_gizmo_show(scene);
      }
    }

    ImGui::Spacing();
  }

  ImGui::End();
}
