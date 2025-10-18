#include "inspector.information.hpp"
#include "imgui/imgui.h"
#include "runtime/scene/editor/ui/components/spacing.hpp"
#include "runtime/scene/editor/ui/components/tree_item.hpp"
#include "runtime/scene/editor/ui/windows/display.hpp"
#include "runtime/scene/editor/ui/windows/registry.hpp"

void UI::InfoTab::draw() {

  uint8_t i;

  UI::Spacing(ui, SceneEditorUISize_Space_Medium).draw_y();
  if (ImGui::Button("Registry data"))
    UI::Registry::open = true;

  // === Scene Stats Info ===
  const struct {
    const char *label;
    const StatCount value;
  } stats_items[] = {
      {"Vertex Count", StatCount_Vertex},
      {"Texture Count", StatCount_Texture},
      {"Shader Count", StatCount_Shader},
      {"Draw Call Count", StatCount_DrawCall},
  };

  UI::Spacing(ui, SceneEditorUISize_Space_Medium).draw_y();
  if (UI::TreeItem(scene, "Scene").draw()) {
    for (i = 0; i < sizeof(stats_items) / sizeof(stats_items[0]); i++) {
      ImGui::Spacing();
      ImGui::PushTextWrapPos(ImGui::GetCursorPosX() +
                             ImGui::GetContentRegionAvail().x);
      ImGui::Text("%s: %d", stats_items[i].label,
                  stat_get_count(&scene->renderer.stats, stats_items[i].value));
      ImGui::PopTextWrapPos();
      ImGui::Separator();
    }
    ImGui::TreePop();
  }

  // === Device Info ===
  const struct {
    const char *label;
    const char *value;
  } device_items[] = {
      {"Vendor", g_context.adapter_info.vendor},
      {"Architecture", g_context.adapter_info.architecture},
      {"Device", g_context.adapter_info.device},
      {"Description", g_context.adapter_info.description},
      {"Backend", backend_label[g_context.adapter_info.backendType]},
      {"Adapter Type", adapter_type_label[g_context.adapter_info.adapterType]},
  };

  UI::Spacing(ui, SceneEditorUISize_Space_Small).draw_y();
  if (UI::TreeItem(scene, "Device").draw()) {
    for (i = 0; i < sizeof(device_items) / sizeof(device_items[0]); i++) {
      ImGui::Spacing();
      ImGui::PushTextWrapPos(ImGui::GetCursorPosX() +
                             ImGui::GetContentRegionAvail().x);
      ImGui::Text("%s: %s", device_items[i].label, device_items[i].value);
      ImGui::PopTextWrapPos();
      ImGui::Separator();
    }
    ImGui::TreePop();
  }
}
