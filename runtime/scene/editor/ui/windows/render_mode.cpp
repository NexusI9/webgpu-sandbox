#include "render_mode.hpp"
#include "imgui/imgui.h"
#include "runtime/scene/draw.h"
#include "runtime/scene/editor/ui/components/button_icon.hpp"

static const struct {
  const SceneRendererDrawMode mode;
  const char *label;
  const SceneEditorUIIcon icon;
  const char *tooltip;
} render_button[] = {
    {
        SceneRendererDrawMode_Boundbox,
        "Boundbox",
        SceneEditorUIIcon_RenderMode_Boundbox,
        "Boundbox rendering",
    },
    {
        SceneRendererDrawMode_Wireframe,
        "Wireframe",
        SceneEditorUIIcon_RenderMode_Wireframe,
        "Wireframe rendering",
    },
    {
        SceneRendererDrawMode_Solid,
        "Solid",
        SceneEditorUIIcon_RenderMode_Solid,
        "Solid rendering",
    },
    {
        SceneRendererDrawMode_Texture,
        "Texture",
        SceneEditorUIIcon_RenderMode_Texture,
        "Texture rendering",
    },
};

void UI::RenderMode::draw() {

  for (uint8_t i = 0; i < 4; i++) {

    bool render_mode_button =
        ButtonIcon(ui, render_button[i].icon, render_button[i].label,
                   ImVec2(ui->size[SceneEditorUISize_Button_RenderModeSize],
                          ui->size[SceneEditorUISize_Button_RenderModeSize]))
            .draw();

    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("%s", render_button[i].tooltip);

    if (render_mode_button)
      scene_set_draw_mode(scene, render_button[i].mode);

    ImGui::SameLine();
  }
}
