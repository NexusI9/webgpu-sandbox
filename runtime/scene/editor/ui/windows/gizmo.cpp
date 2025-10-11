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

static void scene_editor_ui_update_gizmo_mode(Scene *scene, void *mode) {

  scene_gizmo_hide(scene);
  scene->editor.gizmo.transform.mode = *(GizmoMode *)mode;
  if (scene_selection_length(&scene->editor.selection)) {
    scene_gizmo_pos_to_selection(&scene->editor.gizmo.transform,
                                 &scene->editor.selection,
                                 &scene->renderer.ssbo);
    scene_gizmo_show(scene);
  }
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

  GizmoMode mode_position = GizmoMode_Position;
  GizmoMode mode_rotation = GizmoMode_Rotation;
  GizmoMode mode_scale = GizmoMode_Scale;

  UI::ButtonGroup(scene, label,
                  (ButtonGroupItem[]){
                      {
                          "gizmo_button_position",
                          SceneEditorUIIcon_Gizmo_Position,
                          scene_editor_ui_update_gizmo_mode,
                          (void *)&mode_position,
                      },
                      {
                          "gizmo_button_rotate",
                          SceneEditorUIIcon_Gizmo_Rotate,
                          scene_editor_ui_update_gizmo_mode,
                          (void *)&mode_rotation,
                      },
                      {
                          "gizmo_button_scale",
                          SceneEditorUIIcon_Gizmo_Scale,
                          scene_editor_ui_update_gizmo_mode,
                          (void *)&mode_scale,
                      },
                  },
                  3, &style)
      .draw();
}
