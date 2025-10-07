#include "monitor.hpp"
#include "imgui/imgui.h"

typedef enum {
  UIMonitorType_FPS,
  UIMonitorType_CPU,
  UIMonitorType_GPU,
} UIMonitorType;

static float values[3][90] = {};
static int values_offset[3] = {0};

void UI::Monitor::draw() {

  ImGui::SetNextWindowPos(
      ImVec2(ui->size[SceneEditorUISize_Gizmo_Margin],
             ui->size[SceneEditorUISize_TopBar_Height] + 10));
  ImGui::SetNextWindowSize(ImVec2(ui->size[SceneEditorUISize_Monitor_Width],
                                  ui->size[SceneEditorUISize_Monitor_Height]));

  ImGui::Begin(label, nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoBackground);
  // === FPS ===
  {
    float fps = ImGui::GetIO().Framerate;
    float *value = values[UIMonitorType_FPS];
    int *offset = &values_offset[UIMonitorType_FPS];

    value[*offset] = fps;
    *offset = (*offset + 1) % IM_ARRAYSIZE(values[UIMonitorType_FPS]);

    char buf[64];
    snprintf(buf, sizeof(buf), "FPS: %.0f", fps);

    ImGui::PlotLines(buf, value, IM_ARRAYSIZE(values[UIMonitorType_FPS]),
                     *offset, nullptr, 0.0f, 120.0f, ImVec2(0, 80));
  }

  ImGui::End();
}
