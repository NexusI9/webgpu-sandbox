#ifndef _SCENE_EDITOR_UI_STYLE_CARBON_H_
#define _SCENE_EDITOR_UI_STYLE_CARBON_H_
// ht2ps://github.com/GraphicsProgramming/dear-imgui-styles?tab=readme-ov-file

#include "../theme/theme.default.h"
#include "include/imgui/imgui.h"

inline void scene_editor_ui_style_carbon() {
  ImGuiStyle &style = ImGui::GetStyle();

  //style.FontScaleDpi = 2.0f;
  style.FontScaleMain = 1.0f;
  style.Alpha = 1.0;
  // style.WindowFillAlphaDefault = 0.83;
  style.ChildRounding = 3;
  style.WindowRounding = 0;
  style.FrameRounding = 0;
  style.GrabRounding = 1;
  style.GrabMinSize = 20;
  style.FrameRounding = 3;
  style.WindowBorderSize = 0.0f;
  style.ChildBorderSize = 0.0f;
  style.FrameBorderSize = 0.0f;
  style.PopupBorderSize = 0.0f;

  style.ScaleAllSizes(2.0f);

  style.Colors[ImGuiCol_Text] =
      (ImVec4 &)*theme_default_color[THEME_DEFAULT_COLOR_TEXT_ON_DARK];

  style.Colors[ImGuiCol_TextDisabled] =
      (ImVec4 &)*theme_default_color[THEME_DEFAULT_COLOR_TEXT_DISABLED_ON_DARK];

  style.Colors[ImGuiCol_WindowBg] =
      (ImVec4 &)*theme_default_color[THEME_DEFAULT_COLOR_SURFACE_LOWER];

  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_Border] = ImVec4(1.00f, 1.00f, 1.00f, 0.65f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

  style.Colors[ImGuiCol_FrameBg] =
      (ImVec4 &)*theme_default_color[THEME_DEFAULT_COLOR_SURFACE_LOWEST];

  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.03f, 0.03f, 0.03f, 1.0f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.27f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.44f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] =
      ImVec4(1.00f, 1.00f, 1.00f, 0.74f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] =
      ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  // style.Colors[ImGuiCol_ComboBg] = ImVec4(0.16f, 0.24f, 0.22f, 0.60f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 0.68f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.36f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.76f);

  style.Colors[ImGuiCol_Button] =
      (ImVec4 &)*theme_default_color[THEME_DEFAULT_COLOR_SURFACE_BASE];

  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.43f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.62f);
  style.Colors[ImGuiCol_Header] = ImVec4(1.00f, 1.00f, 1.00f, 0.33f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.12f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.54f);
  // style.Colors[ImGuiCol_Column] = ImVec4(0.00f, 0.50f, 0.50f, 0.33f);
  // style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.00f, 0.50f, 0.50f, 0.47f);
  // style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.00f, 0.70f, 0.70f, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.54f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.74f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  // style.Colors[ImGuiCol_CloseButton] = ImVec4(0.00f, 0.78f, 0.78f, 0.35f);
  // style.Colors[ImGuiCol_CloseButtonHovered] =
  //     ImVec4(0.00f, 0.78f, 0.78f, 0.47f);
  // style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.00f, 0.78f,
  // 0.78f, 1.00f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogramHovered] =
      ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.22f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.0f);

  // style.Colors[ImGuiCol_ModalWindowDarkening] =
  //    ImVec4(0.10f, 0.10f, 0.09f, 0.51f);
}

#endif
