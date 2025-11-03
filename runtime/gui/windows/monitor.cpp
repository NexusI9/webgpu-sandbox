#include "monitor.hpp"
#include "backend/theme/core.h"
#include "imgui/imgui.h"

typedef enum {
  GUIMonitorType_FPS,
  GUIMonitorType_CPU,
  GUIMonitorType_GPU,
} GUIMonitorType;

static float values[3][90] = {};
static int values_offset[3] = {0};

void UI::Monitor::draw() {

  ImGui::SetNextWindowPos(
      ImVec2(theme_size(gui->theme, ThemeSize_Gizmo_Margin),
             theme_size(gui->theme, ThemeSize_TopBar_Height) + 10));
  ImGui::SetNextWindowSize(
      ImVec2(theme_size(gui->theme, ThemeSize_Monitor_Width),
             theme_size(gui->theme, ThemeSize_Monitor_Height)));

  ImGui::Begin(label, nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoBackground);
  // === FPS ===
  {
    float fps = ImGui::GetIO().Framerate;
    float *value = values[GUIMonitorType_FPS];
    int *offset = &values_offset[GUIMonitorType_FPS];

    value[*offset] = fps;
    *offset = (*offset + 1) % IM_ARRAYSIZE(values[GUIMonitorType_FPS]);

    char buf[64];
    snprintf(buf, sizeof(buf), "FPS: %.0f", fps);

    ImGui::PlotLines(buf, value, IM_ARRAYSIZE(values[GUIMonitorType_FPS]),
                     *offset, nullptr, 0.0f, 120.0f, ImVec2(0, 80));
  }

  ImGui::End();
}
