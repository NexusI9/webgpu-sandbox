#include "inspector.mesh.hpp"
#include "backend/ssbo.h"
#include "imgui/imgui.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/editor/selection/utils.h"
#include "runtime/scene/editor/ui/components/input.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.hpp"
#include <cstdio>

void UI::InspectorMesh::update_extra_callback(Scene *scene, void *user_data) {
  scene_gizmo_pos_to_selection(&scene->editor.gizmo.transform,
                               &scene->editor.selection, &scene->renderer.ssbo);
}

void UI::InspectorMesh::draw() {

  ImGui::BeginChild("##MeshProp", ImVec2(0, 0), true);
  {

    UI::InputTextCallback<Mesh>(mesh, scene, "Mesh name", sizeof(name_t),
                        InputFlag_SpanFullWidth, mesh_get_name, mesh_set_name,
                        NULL, NULL)
        .draw();

    ImGui::Spacing();
    inspector_tree_list_draw(mesh, &transform_attributes, scene, SSBOType_Mesh,
                             mesh->ssbo_slot.id);
  }
  ImGui::EndChild();
}
