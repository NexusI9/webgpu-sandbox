#include "render_mode.hpp"
#include "backend/renderer/core.h"
#include "imgui/imgui.h"
#include "resources/tool/css2h/output/theme.default.h"
#include "runtime/gui/components/button_group.hpp"
#include "runtime/gui/components/button_icon.hpp"
#include "runtime/gui/core.h"

void UI::RenderMode::draw() {

  ButtonStyle style = {
      .border_radius = 10.0f,
      .inner_padding = 8.0f,
      .size = theme_size(gui->theme, ThemeSize_Button_RenderModeSize),
      .background_default =
          (ImVec4 &)*theme_color(gui->theme, ThemeColor_Surface_Base),
      .background_hover =
          (ImVec4 &)*theme_color(gui->theme, ThemeColor_Surface_High),
      .background_active =
          (ImVec4 &)*theme_color(gui->theme, ThemeColor_Background_Brand_Base),
  };

  UI::ButtonGroup(gui, label, buttons, count, &style,
                  __builtin_ctz(renderer_draw_mode(renderer)),
                  ButtonGroupDirection_Horizontal)
      .draw();
}
