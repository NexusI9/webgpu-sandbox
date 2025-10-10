#include "inspector.mesh.hpp"
#include "backend/ssbo.h"
#include "imgui/imgui.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/editor/ui/windows/inspector/inspector.hpp"
#include <cstdio>

void UI::InspectorMesh::draw() {

  ImGui::BeginChild("##MeshProp", ImVec2(0, 0), true);
  {

    char name_id[128];
    snprintf(name_id, 12, "##%s", mesh->name);

    if (ImGui::InputText(name_id, mesh->name, sizeof(mesh->name))) {
      // mesh_set_name(mesh, mesh->name);
    }

    ImGui::Spacing();
    inspector_tree_list_draw(mesh, &attributes, scene, SSBOType_Mesh,
                             mesh->ssbo_slot.id);
  }
  ImGui::EndChild();
}
