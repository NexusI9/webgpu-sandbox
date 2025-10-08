#include "hierarchy.hpp"
#include "imgui/imgui.h"
#include "runtime/mesh/core.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/selection/core.h"
#include "runtime/scene/editor/ui/components/ButtonIcon.hpp"
#include "runtime/scene/show.h"

void UI::Hierarchy::draw() {

  ImGui::BeginChild("Tree", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.3f),
                    true);
  ImGui::Text("Scene Inspector");

  {
    // === Meshes ===
    MeshRefList *dynamic_meshes[SCENE_DYNAMIC_PIPELINE_COUNT];
    size_t count;
    scene_dynamic_pipelines(scene, dynamic_meshes, &count);

    for (int i = 0; i < count; i++) {
      MeshRefList *meshes = dynamic_meshes[i];
      for (int j = 0; j < meshes->length; j++) {
        Mesh *mesh = meshes->entries[j];

        // ImGuiTreeNodeFlags_SpanAvailWidth
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                                   ImGuiTreeNodeFlags_AllowItemOverlap |
                                   ImGuiTreeNodeFlags_FramePadding;

        if (mesh->children.length == 0)
          flags |= ImGuiTreeNodeFlags_Leaf;

        // Tree item
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                            ImVec2(ui->size[SceneEditorUISize_Tree_PaddingH],
                                   ui->size[SceneEditorUISize_Tree_PaddingV]));
        if (ImGui::TreeNodeEx(mesh->name, flags)) {

          if (ImGui::IsItemClicked())
            scene_selection_toggle_mesh(scene, mesh);

          if (mesh->children.length == 0)
            ImGui::TreePop();
        }
        ImGui::PopStyleVar();

        // Visibility icon
        {
          const float line_height = ImGui::GetTextLineHeightWithSpacing();
          const float icon_size =
              ui->size[SceneEditorUISize_Button_RenderModeSize];

          ImGui::SameLine(ImGui::GetWindowContentRegionMax().x -
                          ui->dpi * icon_size);

          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

          char button_id[256];
          snprintf(button_id, 256, "mesh_visibility_%u", mesh->id);
          if (ButtonIcon(ui, SceneEditorUIIcon_Eye, button_id,
                         ImVec2(icon_size, icon_size))
                  .draw())
            scene_visibility_toggle_mesh(scene, mesh);

          ImGui::PopStyleColor(1);
        }
      }
    }

    // === Lights ===

    // === Probes ===
  }
  ImGui::EndChild();
}
