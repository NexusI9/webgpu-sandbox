#include "log.hpp"

#include "../imgui_style/style.carbon.hpp"
#include "runtime/scene/show.h"

typedef struct {
  const ThemeDefaultColor text;
  const ThemeDefaultColor timestamp;
  const ThemeDefaultColor background;
  SceneEditorUIIcon icon;
} LoggerLook;

static const LoggerLook logger_looks[LOGGER_FLAG_COUNT] = {
    [LoggerFlag_Print] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_ON_DARK,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_SUBTLE_ON_DARK,
            .background = THEME_DEFAULT_COLOR_SURFACE_LOWER,
            .icon = SceneEditorUIIcon_Null,
        },
    [LoggerFlag_Info] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_ON_DARK,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_SUBTLE_ON_DARK,
            .background = THEME_DEFAULT_COLOR_SURFACE_LOWER,
            .icon = SceneEditorUIIcon_Log_Info,
        },
    [LoggerFlag_Error] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_DANGER_ON_DANGER,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_DANGER_SUBTLE,
            .background = THEME_DEFAULT_COLOR_BACKGROUND_DANGER_STRONG_DARK,
            .icon = SceneEditorUIIcon_Log_Error,
        },
    [LoggerFlag_Warning] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_WARNING_ON_WARNING,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_WARNING_SUBTLE,
            .background = THEME_DEFAULT_COLOR_BACKGROUND_WARNING_STRONG_DARK,
            .icon = SceneEditorUIIcon_Log_Warning,
        },
    [LoggerFlag_Debug] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_ON_DARK,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_SUBTLE_ON_DARK,
            .background = THEME_DEFAULT_COLOR_SURFACE_LOWER,
            .icon = SceneEditorUIIcon_Null,
        },
    [LoggerFlag_ShaderCreate] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_ON_DARK,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_SUBTLE_ON_DARK,
            .background = THEME_DEFAULT_COLOR_SURFACE_LOWER,
            .icon = SceneEditorUIIcon_Null,
        },
    [LoggerFlag_MeshBuild] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_ON_DARK,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_SUBTLE_ON_DARK,
            .background = THEME_DEFAULT_COLOR_SURFACE_LOWER,
            .icon = SceneEditorUIIcon_Null,
        },
    [LoggerFlag_MeshCreate] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_ON_DARK,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_SUBTLE_ON_DARK,
            .background = THEME_DEFAULT_COLOR_SURFACE_LOWER,
            .icon = SceneEditorUIIcon_Null,
        },
    [LoggerFlag_Import] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_INFORMATION_ON_INFORMATION,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_INFORMATION_SUBTLE,
            .background =
                THEME_DEFAULT_COLOR_BACKGROUND_INFORMATION_STRONG_DARK,
            .icon = SceneEditorUIIcon_Log_Import,
        },
    [LoggerFlag_Success] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_SUCCESS_ON_SUCCESS,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_SUCCESS_SUBTLE,
            .background = THEME_DEFAULT_COLOR_BACKGROUND_SUCCESS_STRONG_DARK,
            .icon = SceneEditorUIIcon_Log_Success,
        },
    [LoggerFlag_Process] =
        {
            .text = THEME_DEFAULT_COLOR_TEXT_ON_DARK,
            .timestamp = THEME_DEFAULT_COLOR_TEXT_SUBTLE_ON_DARK,
            .background = THEME_DEFAULT_COLOR_SURFACE_LOWER,
            .icon = SceneEditorUIIcon_Log_Process,
        },
};

void UI::Log::draw() {

  static ImVec2 log_icon_scale =
      ImVec2(ui->size[SceneEditorUISize_Log_IconScale],
             ui->size[SceneEditorUISize_Log_IconScale]);

  ImGui::BeginChild("Logs", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0),
                    true);
  {
    ImGui::Text("%s", label);
    ImGui::BeginChild("Logs entries", ImVec2(0, 0), true);
    {
      for (size_t i = 0; i < g_logger.length; i++) {

        const LoggerLook *look = &logger_looks[g_logger.flags[i]];
        const SceneEditorUIIconUV *uv = &ui->icon_uv[look->icon];
        const color *background_color = &theme_default_color[look->background];
        const color *timestamp_color = &theme_default_color[look->timestamp];
        const color *text_color = &theme_default_color[look->text];
        const char *message = g_logger.messages[i];

        ImVec2 pos = ImGui::GetCursorScreenPos();

        float row_height = ImGui::GetTextLineHeightWithSpacing();
        float row_width = ImGui::GetContentRegionAvail().x;

        ImGui::GetWindowDrawList()->AddRectFilled(
            pos, ImVec2(pos.x + row_width, pos.y + row_height),
            ImGui::ColorConvertFloat4ToU32((ImVec4 &)*background_color));

        ImGui::Image((ImTextureRef)ui->atlas_texture.view, log_icon_scale,
                     ImVec2(uv->uv0[0], uv->uv0[1]),
                     ImVec2(uv->uv1[0], uv->uv1[1]));
        ImGui::SameLine();
        ImGui::TextColored((ImVec4 &)*text_color, "%s", g_logger.messages[i]);
      }

      if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
  }
  ImGui::EndChild();
}
