#include "inspector.mesh.hpp"
#include "backend/ubo.h"
#include "imgui/imgui.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/editor/selection/utils.h"
#include "runtime/scene/editor/ui/components/input.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.hpp"
#include <cstdio>

void UI::InspectorMesh::transform_update_callback(Scene *scene,
                                                  void *user_data) {

  ubo_update_queue_insert(&scene->renderer.ubo, UBOType_Mesh,
                           ((Mesh *)user_data)->ubo_slot.id);

  scene_gizmo_pos_to_selection(&scene->editor.gizmo.transform,
                               &scene->editor.selection, &scene->renderer.ubo);
}

void UI::InspectorMesh::draw() {

  ImGui::BeginChild("##Prop", ImVec2(0, 0), true);
  {

    UI::InputTextCallback<Mesh>(mesh, scene, "Mesh name", sizeof(name_t),
                                InputFlag_SpanFullWidth, mesh_get_name,
                                mesh_set_name, NULL, NULL)
        .draw();

    inspector_tree_list_draw(mesh, &transform_attributes, scene);
  }
  ImGui::EndChild();
}
