#include "log.hpp"

#include "../imgui_style/style.carbon.hpp"
#include "backend/theme/core.h"
#include "imgui/imgui.h"

typedef struct {
  const ThemeColor text;
  const ThemeColor timestamp;
  const ThemeColor background;
  ThemeIcon icon;
} LoggerLook;

static const LoggerLook logger_looks[LOGGER_FLAG_COUNT] = {
    [LoggerFlag_Print] =
        {
            .text = ThemeColor_Text_On_Dark,
            .timestamp = ThemeColor_Text_Subtle_On_Dark,
            .background = ThemeColor_Surface_Lower,
            .icon = ThemeIcon_Null,
        },
    [LoggerFlag_Info] =
        {
            .text = ThemeColor_Text_On_Dark,
            .timestamp = ThemeColor_Text_Subtle_On_Dark,
            .background = ThemeColor_Surface_Lower,
            .icon = ThemeIcon_Log_Info,
        },
    [LoggerFlag_Error] =
        {
            .text = ThemeColor_Text_Danger_On_Danger,
            .timestamp = ThemeColor_Text_Danger_Subtle,
            .background = ThemeColor_Background_Danger_Strong_Dark,
            .icon = ThemeIcon_Log_Error,
        },
    [LoggerFlag_Warning] =
        {
            .text = ThemeColor_Text_Warning_On_Warning,
            .timestamp = ThemeColor_Text_Warning_Subtle,
            .background = ThemeColor_Background_Warning_Strong_Dark,
            .icon = ThemeIcon_Log_Warning,
        },
    [LoggerFlag_Debug] =
        {
            .text = ThemeColor_Text_On_Dark,
            .timestamp = ThemeColor_Text_Subtle_On_Dark,
            .background = ThemeColor_Surface_Lower,
            .icon = ThemeIcon_Null,
        },
    [LoggerFlag_ShaderCreate] =
        {
            .text = ThemeColor_Text_On_Dark,
            .timestamp = ThemeColor_Text_Subtle_On_Dark,
            .background = ThemeColor_Surface_Lower,
            .icon = ThemeIcon_Null,
        },
    [LoggerFlag_MeshBuild] =
        {
            .text = ThemeColor_Text_On_Dark,
            .timestamp = ThemeColor_Text_Subtle_On_Dark,
            .background = ThemeColor_Surface_Lower,
            .icon = ThemeIcon_Null,
        },
    [LoggerFlag_MeshCreate] =
        {
            .text = ThemeColor_Text_On_Dark,
            .timestamp = ThemeColor_Text_Subtle_On_Dark,
            .background = ThemeColor_Surface_Lower,
            .icon = ThemeIcon_Null,
        },
    [LoggerFlag_Import] =
        {
            .text = ThemeColor_Text_Information_On_Information,
            .timestamp = ThemeColor_Text_Information_Subtle,
            .background = ThemeColor_Background_Information_Strong_Dark,
            .icon = ThemeIcon_Log_Import,
        },
    [LoggerFlag_Success] =
        {
            .text = ThemeColor_Text_Success_On_Success,
            .timestamp = ThemeColor_Text_Success_Subtle,
            .background = ThemeColor_Background_Success_Strong_Dark,
            .icon = ThemeIcon_Log_Success,
        },
    [LoggerFlag_Process] =
        {
            .text = ThemeColor_Text_On_Dark,
            .timestamp = ThemeColor_Text_Subtle_On_Dark,
            .background = ThemeColor_Surface_Lower,
            .icon = ThemeIcon_Log_Process,
        },
};

void UI::Log::draw() {

  static ImVec2 log_icon_scale =
      ImVec2(theme_size(gui->theme, ThemeSize_Log_IconScale),
             theme_size(gui->theme, ThemeSize_Log_IconScale));

  ImGui::BeginChild("Logs", ImVec2(ImGui::GetContentRegionAvail().x, 0), true);
  {
    ImGui::Text("%s", label);
    ImGui::BeginChild("Logs entries", ImVec2(0, 0), true);
    {
      for (size_t i = 0; i < g_logger.length; i++) {

        const LoggerLook *look = &logger_looks[g_logger.flags[i]];
        const ThemeIconCell *uv = theme_icon_cell(gui->theme, look->icon);
        const float *background_color =
            theme_color(gui->theme, look->background);
        const float *timestamp_color = theme_color(gui->theme, look->timestamp);
        const float *text_color = theme_color(gui->theme, look->text);
        const char *message = g_logger.messages[i];

        ImVec2 pos = ImGui::GetCursorScreenPos();

        float row_height = ImGui::GetTextLineHeightWithSpacing();
        float row_width = ImGui::GetContentRegionAvail().x;

        ImGui::GetWindowDrawList()->AddRectFilled(
            pos, ImVec2(pos.x + row_width, pos.y + row_height),
            ImGui::ColorConvertFloat4ToU32((ImVec4 &)*background_color));

        ImGui::Image((ImTextureRef)theme_icon_atlas(gui->theme), log_icon_scale,
                     ImVec2(uv->uv0[0], uv->uv0[1]),
                     ImVec2(uv->uv1[0], uv->uv1[1]));
        ImGui::SameLine();
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() +
                               ImGui::GetContentRegionAvail().x);
        ImGui::TextColored((ImVec4 &)*text_color, "%s", g_logger.messages[i]);
        ImGui::PopTextWrapPos();
      }

      if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    }
    ImGui::EndChild();
  }
  ImGui::EndChild();
}
