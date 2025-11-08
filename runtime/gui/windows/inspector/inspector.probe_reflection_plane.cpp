#include "inspector.probe_reflection_plane.hpp"
#include "backend/ubo.h"
#include "imgui/imgui.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/light/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor_mesh/list.h"
#include "runtime/systems/selection_system.h"
#include <cstdio>

void UI::InspectorProbeReflectionPlane::transform_update_callback(
    Scene *scene, Renderer *renderer, void *user_data) {

  SceneEditorMeshList *list = (SceneEditorMeshList *)user_data;

  for (size_t i = 0; i < list->length; i++)
    ubo_update_queue_insert(scene->ubo, UBOType_Mesh,
                            list->entries[i].mesh->ubo_slot.id);

  selection_system_update_gizmo_pos_to_selection(&scene->gizmo,
                                                 &scene->selection, scene->ubo);
}

void UI::InspectorProbeReflectionPlane::properties_update_callback(
    Scene *scene, Renderer *renderer, void *user_data) {

  SceneEditorMeshList *list = (SceneEditorMeshList *)user_data;
  ProbeReflectionPlane *probe = (ProbeReflectionPlane *)list->origin->target;

  ubo_update_queue_insert(scene->ubo, UBOType_ProbeList,
                          scene->probes.ubo_slot.id);
}

void UI::InspectorProbeReflectionPlane::draw() {

  ImGui::BeginChild("##Prop", ImVec2(0, 0), true);
  {

    probe = (ProbeReflectionPlane *)sem->origin->target;

    UI::InputTextCallback<SceneEditorMeshList>(
        sem, gui, "Probe name", sizeof(name_t), InputFlag_SpanFullWidth,
        sem_list_get_name, sem_list_set_name, NULL, NULL)
        .draw();

    inspector_tree_list_draw(sem, &transform_attributes, gui);
  }
  ImGui::EndChild();
}
