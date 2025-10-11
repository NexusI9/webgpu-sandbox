#include "inspector.ambient_light.hpp"
#include "backend/ssbo.h"
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
    inspector_tree_list_draw(light, &properties_attributes, scene, SSBOType_AmbientLight,
                             light->ssbo_slot.id);
  }
  ImGui::EndChild();
}
