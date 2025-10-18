#include "registry.hpp"
#include "backend/registry.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_wgpu.h"
#include "runtime/scene/editor/ui/core.h"
#include "runtime/scene/editor/ui/windows/inspector/inspector.information.hpp"

bool UI::Registry::open = false;

void UI::Registry::draw() {

  ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
  if (ImGui::Begin("Registry", &UI::Registry::open,
                   ImGuiWindowFlags_NoCollapse)) {

    if (ImGui::BeginTable("RegistryTable", 4,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
      ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("Type");
      ImGui::TableSetupColumn("Type #", ImGuiTableColumnFlags_WidthFixed);
      ImGui::TableSetupColumn("Address");
      ImGui::TableHeadersRow();

      for (int i = 1; i < reg_length(); i++) {

        const RegEntry *entry = reg_lookup(i);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("%d", entry->id);
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", reg_label[entry->type]);
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%d", entry->type);
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("%p", entry->ptr);
      }

      ImGui::EndTable();
    }

    ImGui::End();
  }
}
