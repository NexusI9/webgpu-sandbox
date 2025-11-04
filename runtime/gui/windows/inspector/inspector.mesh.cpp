#include "inspector.mesh.hpp"
#include "backend/ubo.h"
#include "imgui/imgui.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/gui/components/input.hpp"
#include "runtime/gui/components/spacing.hpp"
#include "runtime/gui/windows/inspector/inspector.hpp"
#include "runtime/gui/windows/vertex_viewer.hpp"
#include "runtime/mesh/transform.h"
#include "runtime/scene/editor/selection/utils.h"
#include <cstdio>

void UI::InspectorMesh::transform_update_callback(Scene *scene,
                                                  void *user_data) {

  ubo_update_queue_insert(scene->ubo, UBOType_Mesh,
                          ((Mesh *)user_data)->ubo_slot.id);

  scene_gizmo_pos_to_selection(&scene->editor.gizmo.transform,
                               &scene->editor.selection, scene->ubo);
}

void UI::InspectorMesh::draw() {

  ImGui::BeginChild("##Prop", ImVec2(0, 0), true);
  {

    UI::InputTextCallback<Mesh>(mesh, gui, "Mesh name", sizeof(name_t),
                                InputFlag_SpanFullWidth, mesh_get_name,
                                mesh_set_name, NULL, NULL)
        .draw();

    inspector_tree_list_draw(mesh, &transform_attributes, gui);

    UI::Spacing(gui, ThemeSize_Space_Medium).draw_y();
    ImGui::Text("Vertex info");
    ImGui::SameLine();
    if (ImGui::Button("Open table"))
      UI::VertexViewer::open = true;
  }
  ImGui::EndChild();
}
