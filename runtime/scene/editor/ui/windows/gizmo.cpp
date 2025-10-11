#include "gizmo.hpp"
#include "imgui/imgui.h"
#include "resources/tool/css2h/output/theme.default.h"
#include "runtime/scene/editor/selection/core.h"
#include "runtime/scene/editor/selection/gizmo/core.h"
#include "runtime/scene/editor/selection/utils.h"
#include "runtime/scene/editor/ui/components/button_group.hpp"
#include "runtime/scene/editor/ui/components/button_icon.hpp"
#include "runtime/scene/editor/ui/core.h"

static const struct {
  const GizmoMode mode;
  const char *label;
  const SceneEditorUIIcon icon;
} gizmo_button[] = {
    {GizmoMode_Position, "Position", SceneEditorUIIcon_Gizmo_Position},
    {GizmoMode_Rotation, "Rotate", SceneEditorUIIcon_Gizmo_Rotate},
    {GizmoMode_Scale, "Scale", SceneEditorUIIcon_Gizmo_Scale},
};

static void scene_editor_ui_update_gizmo_mode(Scene *scene,
                                              const GizmoMode mode) {
  scene_gizmo_hide(scene);
  scene->editor.gizmo.transform.mode = mode;
  if (scene_selection_length(&scene->editor.selection)) {
    scene_gizmo_pos_to_selection(&scene->editor.gizmo.transform,
                                 &scene->editor.selection,
                                 &scene->renderer.ssbo);
    scene_gizmo_show(scene);
  }
}

static void scene_editor_ui_update_gizmo_mode_position(Scene *scene) {
  scene_editor_ui_update_gizmo_mode(scene, GizmoMode_Position);
}
static void scene_editor_ui_update_gizmo_mode_rotation(Scene *scene) {
  scene_editor_ui_update_gizmo_mode(scene, GizmoMode_Rotation);
}
static void scene_editor_ui_update_gizmo_mode_scale(Scene *scene) {
  scene_editor_ui_update_gizmo_mode(scene, GizmoMode_Scale);
}

void UI::Gizmo::draw() {

  ImGui::SetNextWindowPos(
      ImVec2(ui->size[SceneEditorUISize_Gizmo_Margin],
             (int)(ui->size[SceneEditorUISize_Screen_Height] / 2) -
                 (int)(ui->size[SceneEditorUISize_Gizmo_Height] / 2)));

  ButtonStyle style = {
      .border_radius = 20.0f,
      .inner_padding = 10.0f,
      .size = ui->size[SceneEditorUISize_Button_GizmoSize],
      .background_default =
          (ImVec4 &)theme_default_color[THEME_DEFAULT_COLOR_SURFACE_BASE],
      .background_hover =
          (ImVec4 &)theme_default_color[THEME_DEFAULT_COLOR_SURFACE_HIGH],

  };

  UI::ButtonGroup(scene, label,
                  (ButtonGroupItem[]){
                      {
                          "gizmo_button_position",
                          SceneEditorUIIcon_Gizmo_Position,
                          scene_editor_ui_update_gizmo_mode_position,
                      },
                      {
                          "gizmo_button_rotate",
                          SceneEditorUIIcon_Gizmo_Rotate,
                          scene_editor_ui_update_gizmo_mode_rotation,
                      },
                      {
                          "gizmo_button_scale",
                          SceneEditorUIIcon_Gizmo_Scale,
                          scene_editor_ui_update_gizmo_mode_scale,
                      },
                  },
                  3, &style)
      .draw();

  // for (uint8_t i = 0; i < 3; i++) {
  //   ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
  //   if (ButtonIcon(ui, gizmo_button[i].icon, gizmo_button[i].label,
  //                  ImVec2(ui->size[SceneEditorUISize_Button_GizmoSize],
  //                         ui->size[SceneEditorUISize_Button_GizmoSize]))
  //           .draw()) {
  //     scene_gizmo_hide(scene);
  //     scene->editor.gizmo.transform.mode = gizmo_button[i].mode;
  //     if (scene_selection_length(&scene->editor.selection)) {
  //       scene_gizmo_pos_to_selection(&scene->editor.gizmo.transform,
  //                                    &scene->editor.selection,
  //                                    &scene->renderer.ssbo);
  //       scene_gizmo_show(scene);
  //     }
  //   }
  //   ImGui::PopStyleVar(1);
  // }
}
