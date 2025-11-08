#include "gizmo.hpp"
#include "backend/context.h"
#include "backend/theme/core.h"
#include "imgui/imgui.h"
#include "resources/tool/css2h/output/theme.default.h"
#include "runtime/gui/components/button_group.hpp"
#include "runtime/gui/components/button_icon.hpp"
#include "runtime/gui/core.h"
#include "runtime/systems/gizmo_system.h"
#include "runtime/systems/selection_system.h"

static const struct {
  const GizmoMode mode;
  const char *label;
  const ThemeIcon icon;
} gizmo_button[] = {
    {GizmoMode_Position, "Position", ThemeIcon_Gizmo_Position},
    {GizmoMode_Rotation, "Rotate", ThemeIcon_Gizmo_Rotate},
    {GizmoMode_Scale, "Scale", ThemeIcon_Gizmo_Scale},
};

static void gui_update_gizmo_mode(Scene *scene, Renderer *renderer,
                                  void *mode) {

  gizmo_system_hide(&scene->gizmo, renderer);
  gizmo_set_mode(&scene->gizmo, *(GizmoMode *)mode);

  if (scene_selection_length(&scene->selection)) {
    selection_system_update_gizmo_pos_to_selection(
        &scene->gizmo, &scene->selection, scene->ubo);
    gizmo_system_show(&scene->gizmo, renderer);
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
                  3, &style, scene->gizmo.mode)
      .draw();
}
