#include "inspector.mesh.hpp"
#include "imgui/imgui.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/mesh/transform.h"
#include <cstdio>

void UI::InspectorMesh::draw() {

  ImGui::BeginChild("##MeshProp", ImVec2(0, 0), true);
  {

    char name_id[128];
    snprintf(name_id, 12, "##%s", mesh->name);

    if (ImGui::InputText(name_id, mesh->name, 12)) {
    }

    ImGui::Spacing();

    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                        ImVec2(ui->size[SceneEditorUISize_Tree_PaddingH],
                               ui->size[SceneEditorUISize_Tree_PaddingV]));
    // resolution / multisample
    if (ImGui::TreeNodeEx("Transformation", flags)) {

      for (uint8_t i = 0; i < transform_layout_len; i++) {
        const InspectorMeshTransformLayout *layout = &transform_layout[i];
        ImGui::Text("%s", layout->label);
        layout->accessor_callback(mesh, (float *)layout->attribute);

        for (uint8_t i = 0; i < 3; i++) {

          char input_id[12];
          snprintf(input_id, 12, "##%s%d", layout->label, i);

          if (ImGui::InputFloat(input_id, (float *)&layout->attribute[i]))
            layout->mutator_callback(mesh, (float *)layout->attribute);
        }

        ImGui::Spacing();
      }
      ImGui::TreePop();
    }
    ImGui::PopStyleVar();
  }
  ImGui::EndChild();
}
