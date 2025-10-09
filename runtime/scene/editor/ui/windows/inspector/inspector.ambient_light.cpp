#include "inspector.ambient_light.hpp"
#include "imgui/imgui.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/mesh/transform.h"
#include <cstdio>

void UI::InspectorAmbientLight::draw() {

  ImGui::BeginChild("##MeshProp", ImVec2(0, 0), true);
  {

    name_t name_id;
    snprintf(name_id, 12, "##%s", light->name);

    if (ImGui::InputText(name_id, light->name, sizeof(light->name))) {
      // mesh_set_name(mesh, mesh->name);
    }

    ImGui::Spacing();

    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                        ImVec2(ui->size[SceneEditorUISize_Tree_PaddingH],
                               ui->size[SceneEditorUISize_Tree_PaddingV]));
    // resolution / multisample
    if (ImGui::TreeNodeEx("Properties", flags)) {
      for(int i = 0; i < attr_float_count; i++){
	
      }
      ImGui::TreePop();
    }
    ImGui::PopStyleVar();
  }
  ImGui::EndChild();
}
