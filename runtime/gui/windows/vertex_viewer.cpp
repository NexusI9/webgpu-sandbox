#include "runtime/gui/windows/vertex_viewer.hpp"
#include "backend/registry.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_wgpu.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/geometry/vertex/core.h"
#include "runtime/gui/core.h"
#include "runtime/gui/windows/inspector/inspector.information.hpp"
#include "runtime/gui/windows/inspector/inspector.object.hpp"
#include "utils/name.h"

bool UI::VertexViewer::open = false;

void UI::VertexViewer::draw() {

  static const struct {
    const char *header;
    const char *columns[4];
    const uint8_t count;
    const uint8_t offset;
  } v_data[] = {
      {
          .header = "Position",
          .columns = {"X", "Y", "Z"},
          .count = VertexAttributeDimension_Position,
          .offset = VertexAttributeOffset_Position,
      },
      {
          .header = "Normal",
          .columns = {"X", "Y", "Z"},
          .count = VertexAttributeDimension_Normal,
          .offset = VertexAttributeOffset_Normal,
      },
      {
          .header = "Tangent",
          .columns = {"X", "Y", "Z", "W"},
          .count = VertexAttributeDimension_Tangent,
          .offset = VertexAttributeOffset_Tangent,
      },
      {
          .header = "Color",
          .columns = {"R", "G", "B"},
          .count = VertexAttributeDimension_Color,
          .offset = VertexAttributeOffset_Color,
      },
      {
          .header = "Texture Coordinate",
          .columns = {"U", "V"},
          .count = VertexAttributeDimension_Uv,
          .offset = VertexAttributeOffset_Uv,
      },
  };

  if (UI::ObjectTab::active_object &&
      UI::ObjectTab::active_object->type == RegEntryType_Mesh &&
      UI::VertexViewer::open) {

    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);

    Mesh *mesh = (Mesh *)UI::ObjectTab::active_object->ptr;
    name_t window_name;
    name_compose(window_name, "Vertex Viewer: %s", mesh->name);

    if (ImGui::Begin(window_name, &UI::VertexViewer::open,
                     ImGuiWindowFlags_NoCollapse)) {

      if (ImGui::BeginTable("VertexTable", VERTEX_ATTRIBUTE_COUNT,
                            ImGuiTableFlags_BordersInnerV)) {

        for (int i = 0; i < VERTEX_ATTRIBUTE_COUNT; i++)
          ImGui::TableSetupColumn(v_data[i].header);
        ImGui::TableHeadersRow();

        for (int i = 0; i < VERTEX_ATTRIBUTE_COUNT; i++) {

          ImGui::TableNextColumn();

          name_t inner_table_id;
          name_compose(inner_table_id, "%sinner", v_data[i].header);

          if (ImGui::BeginTable(inner_table_id, v_data[i].count,
                                ImGuiTableFlags_Borders |
                                    ImGuiTableFlags_RowBg)) {
            int j;
            for (j = 0; j < v_data[i].count; j++)
              ImGui::TableSetupColumn(v_data[i].columns[j]);
            ImGui::TableHeadersRow();

            for (j = 0; j < mesh->topology.base.attribute.length;
                 j += VERTEX_STRIDE) {
              ImGui::TableNextRow();

              for (int k = 0; k < v_data[i].count; k++) {
                ImGui::TableSetColumnIndex(k);
                ImGui::Text("%f", mesh->topology.base.attribute
                                      .entries[j + v_data[i].offset + k]);
              }
            }
            ImGui::EndTable();
          }
        }

        ImGui::EndTable();
      }

      ImGui::End();
    }
  }
}
