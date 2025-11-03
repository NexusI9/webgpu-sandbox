#include "gizmo.hpp"
#include "backend/context.h"
#include "backend/theme/core.h"
#include "imgui/imgui.h"
#include "resources/tool/css2h/output/theme.default.h"
#include "runtime/gui/components/button_group.hpp"
#include "runtime/gui/components/button_icon.hpp"
#include "runtime/gui/core.h"
#include "runtime/scene/editor/selection/core.h"
#include "runtime/scene/editor/selection/gizmo/core.h"
#include "runtime/scene/editor/selection/utils.h"

static const struct {
  const GizmoMode mode;
  const char *label;
  const ThemeIcon icon;
} gizmo_button[] = {
    {GizmoMode_Position, "Position", ThemeIcon_Gizmo_Position},
    {GizmoMode_Rotation, "Rotate", ThemeIcon_Gizmo_Rotate},
    {GizmoMode_Scale, "Scale", ThemeIcon_Gizmo_Scale},
};

static void gui_update_gizmo_mode(Scene *scene, void *mode) {

  scene_gizmo_hide(scene);
  scene->editor.gizmo.transform.mode = *(GizmoMode *)mode;
  if (scene_selection_length(&scene->editor.selection)) {
    scene_gizmo_pos_to_selection(&scene->editor.gizmo.transform,
                                 &scene->editor.selection,
                                 &scene->renderer.ubo);
    scene_gizmo_show(scene);
  }
}

void UI::Gizmo::draw() {

  ImGui::SetNextWindowPos(
      ImVec2(theme_size(gui->theme, ThemeSize_Gizmo_Margin),
             (int)(theme_scale_size(gui->theme, context_height()) / 2) -
                 (int)(theme_size(gui->theme, ThemeSize_Gizmo_Height) / 2)));

  ButtonStyle style = {
      .border_radius = 20.0f,
      .inner_padding = 10.0f,
      .size = theme_size(gui->theme, ThemeSize_Button_GizmoSize),
      .background_default =
          (ImVec4 &)*theme_color(gui->theme, ThemeColor_Surface_Base),
      .background_hover =
          (ImVec4 &)*theme_color(gui->theme, ThemeColor_Surface_High),
      .background_active =
          (ImVec4 &)*theme_color(gui->theme, ThemeColor_Background_Brand_Base),
  };

  GizmoMode mode_position = GizmoMode_Position;
  GizmoMode mode_rotation = GizmoMode_Rotation;
  GizmoMode mode_scale = GizmoMode_Scale;

  UI::ButtonGroup(gui, label,
                  (ButtonGroupItem[]){
                      {
                          "gizmo_button_position",
                          ThemeIcon_Gizmo_Position,
                          gui_update_gizmo_mode,
                          (void *)&mode_position,
                      },
                      {
                          "gizmo_button_rotate",
                          ThemeIcon_Gizmo_Rotate,
                          gui_update_gizmo_mode,
                          (void *)&mode_rotation,
                      },
                      {
                          "gizmo_button_scale",
                          ThemeIcon_Gizmo_Scale,
                          gui_update_gizmo_mode,
                          (void *)&mode_scale,
                      },
                  },
                  3, &style, scene->editor.gizmo.transform.mode)
      .draw();
}
