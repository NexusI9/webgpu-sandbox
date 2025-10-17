#include "inspector.probe_reflection_plane.hpp"
#include "backend/ssbo.h"
#include "imgui/imgui.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/light/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/mesh/list/list.h"
#include "runtime/scene/editor/selection/utils.h"
#include <cstdio>

void UI::InspectorProbeReflectionPlane::transform_update_callback(Scene *scene,
                                                          void *user_data) {

  SceneEditorMeshList *list = (SceneEditorMeshList *)user_data;

  for (size_t i = 0; i < list->length; i++)
    ssbo_update_queue_insert(&scene->renderer.ssbo, SSBOType_Mesh,
                             list->entries[i].mesh->ssbo_slot.id);

  scene_gizmo_pos_to_selection(&scene->editor.gizmo.transform,
                               &scene->editor.selection, &scene->renderer.ssbo);
}

void UI::InspectorProbeReflectionPlane::properties_update_callback(Scene *scene,
                                                           void *user_data) {

  SceneEditorMeshList *list = (SceneEditorMeshList *)user_data;
  AmbientLight *light = (AmbientLight *)list->origin->target;

  ssbo_update_queue_insert(&scene->renderer.ssbo, SSBOType_AmbientLight,
                           light->ssbo_slot.id);
}

void UI::InspectorProbeReflectionPlane::draw() {

  ImGui::BeginChild("##Prop", ImVec2(0, 0), true);
  {

    probe = (ProbeReflectionPlane *)sem->origin->target;

    UI::InputTextCallback<SceneEditorMeshList>(
        sem, scene, "Probe name", sizeof(name_t), InputFlag_SpanFullWidth,
        sem_list_get_name, sem_list_set_name, NULL, NULL)
        .draw();

    inspector_tree_list_draw(sem, &transform_attributes, scene);

  }
  ImGui::EndChild();
}
