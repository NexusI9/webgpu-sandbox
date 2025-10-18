#include "inspector.hpp"
#include "backend/context.h"
#include "backend/profiler.h"
#include "backend/registry.h"
#include "backend/stat.h"
#include "backend/std_pipeline/core.h"
#include "imgui/imgui.h"
#include "resources/tool/css2h/output/theme.default.h"
#include "runtime/light/core.h"
#include "runtime/mesh/core.h"
#include "runtime/scene/core.h"
#include "runtime/scene/draw.h"
#include "runtime/scene/editor/selection/core.h"
#include "runtime/scene/editor/ui/components/button_icon.hpp"
#include "runtime/scene/editor/ui/components/input.hpp"
#include "runtime/scene/editor/ui/components/spacing.hpp"
#include "runtime/scene/editor/ui/components/time_bar.hpp"
#include "runtime/scene/editor/ui/components/tree_item.hpp"
#include "runtime/scene/editor/ui/core.h"
#include "runtime/scene/editor/ui/windows/inspector/inspector.ambient_light.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.mesh.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.point_light.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.probe_reflection_plane.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.spot_light.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.sun_light.hpp"
#include "webgpu/webgpu.h"
#include <cstdio>
#include <cstdlib>

int UI::Inspector::active_tab = 0;

void UI::Inspector::draw() {

  const float page_width = ui->size[SceneEditorUISize_RightPanel_Width];
  const float bar_width = ui->size[SceneEditorUISize_RightPanelTab_Width];
  const float gap = scene_editor_ui_size(ui, 4.f);
  const float btn_size = ui->size[SceneEditorUISize_Button_InspectorTab];
  const ImVec2 icon_size{btn_size, btn_size};

  ImGui::BeginChild("##Properties", ImVec2(0, 0), true);
  {
    ImGui::Separator();
    ImGui::Text("%s", label);
    ImGui::Spacing();
    {
      ImGui::BeginChild("##Properties Content", ImVec2(0, 0), true);
      {
        // tighten horizontal spacing
        ImGuiStyle &style = ImGui::GetStyle();
        float savedSpacingX = style.ItemSpacing.x;
        style.ItemSpacing.x = gap;

        // always-visible icon strip
        ImGui::BeginChild("##icon_bar", ImVec2(bar_width, 0), false,
                          ImGuiWindowFlags_NoScrollbar |
                              ImGuiWindowFlags_NoScrollWithMouse);
        {
          for (int i = 0; i < tab_count; ++i) {

            ImGui::PushID(i);
            bool sel = (active_tab == i);

            // highlight selected tab
            if (sel) {
              ImGui::PushStyleColor(ImGuiCol_Button,
                                    ImVec4(0.30f, 0.44f, 0.60f, 1.f));
              ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                                    ImVec4(0.32f, 0.49f, 0.68f, 1.f));
              ImGui::PushStyleColor(ImGuiCol_Border,
                                    ImVec4(0.80f, 0.80f, 0.90f, 1.f));
            }

            bool pressed =
                ButtonIcon(ui, tabs[i]->icon, tabs[i]->tooltip, icon_size)
                    .draw();

            if (pressed)
              active_tab = i;

            if (ImGui::IsItemHovered() && strlen(tabs[i]->tooltip))
              ImGui::SetTooltip("%s", tabs[i]->tooltip);

            if (sel)
              ImGui::PopStyleColor(3);

            ImGui::PopID();
          }
        }
        ImGui::EndChild();

        ImGui::SameLine();

        // draw content
        if (active_tab >= 0) {
          ImGui::BeginChild("##page",
                            ImVec2(page_width - 2.0f * bar_width - gap, 0),
                            true, ImGuiWindowFlags_NoScrollWithMouse);

          // header
          {
            ImGui::Text("%s", tabs[active_tab]->tooltip);
          }

          tabs[active_tab]->draw();

          ImGui::EndChild();
        }

        style.ItemSpacing.x = savedSpacingX;
      }
      ImGui::EndChild();
    }
    ImGui::EndChild();
  }
}
