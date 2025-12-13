#include "inspector.ambient_light.hpp"
#include "backend/ubo.h"
#include "imgui/imgui.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/light/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor_mesh/list.h"
#include "runtime/systems/selection_system.h"

void UI::InspectorAmbientLight::transform_update_callback(Scene *scene,
                                                          Renderer *renderer,
                                                          void *user_data) {

  SceneEditorMeshList *list = (SceneEditorMeshList *)user_data;

  for (size_t i = 0; i < list->count; i++)
    ubo_update_queue_insert(scene->ubo, UBOType_Mesh,
                            list->entries[i].mesh->ubo_slot.id);

  selection_system_update_gizmo_pos_to_selection(&scene->gizmo,
                                                 &scene->selection, scene->ubo);
}

void UI::InspectorAmbientLight::properties_update_callback(Scene *scene,
                                                           Renderer *renderer,
                                                           void *user_data) {

  SceneEditorMeshList *list = (SceneEditorMeshList *)user_data;
  AmbientLight *light = (AmbientLight *)list->origin->target;

  ubo_update_queue_insert(scene->ubo, UBOType_LightList,
                          scene->lights.ubo_slot.id);
}

void UI::InspectorAmbientLight::draw() {

  ImGui::BeginChild("##Prop", ImVec2(0, 0), true);
  {

    light = (AmbientLight *)sem->origin->target;

    UI::InputTextCallback<SceneEditorMeshList>(
        sem, gui, "Light name", sizeof(name_t), InputFlag_SpanFullWidth,
        sem_list_get_name, sem_list_set_name, NULL, NULL)
        .draw();

    inspector_tree_list_draw(sem, &transform_attributes, gui);

    ImGui::Spacing();
    inspector_tree_list_draw(light, &properties_attributes, gui);
  }
  ImGui::EndChild();
}
