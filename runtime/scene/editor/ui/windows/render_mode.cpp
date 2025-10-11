#include "render_mode.hpp"
#include "imgui/imgui.h"
#include "resources/tool/css2h/output/theme.default.h"
#include "runtime/scene/draw.h"
#include "runtime/scene/editor/ui/components/button_group.hpp"
#include "runtime/scene/editor/ui/components/button_icon.hpp"
#include "runtime/scene/editor/ui/core.h"
#include "runtime/scene/renderer/core.h"

void UI::RenderMode::draw() {

  ButtonStyle style = {
      .border_radius = 10.0f,
      .inner_padding = 8.0f,
      .size = ui->size[SceneEditorUISize_Button_RenderModeSize],
      .background_default =
          (ImVec4 &)theme_default_color[THEME_DEFAULT_COLOR_SURFACE_BASE],
      .background_hover =
          (ImVec4 &)theme_default_color[THEME_DEFAULT_COLOR_SURFACE_HIGH],
  };

  UI::ButtonGroup(scene, label, buttons, count, &style,
                  ButtonGroupDirection_Horizontal)
      .draw();
}
