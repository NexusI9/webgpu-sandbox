#include "inspector.point_light.hpp"
#include "backend/registry.h"
#include "backend/ubo.h"
#include "imgui/imgui.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/light/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/editor_mesh/list.h"
#include "runtime/systems/selection_system.h"

void UI::InspectorPointLight::transform_update_callback(Scene *scene,
                                                        Renderer *renderer,
                                                        void *user_data) {

  SceneEditorMeshList *list = (SceneEditorMeshList *)user_data;

  for (size_t i = 0; i < list->length; i++)
    ubo_update_queue_insert(scene->ubo, UBOType_Mesh,
                            list->entries[i].mesh->ubo_slot.id);

  selection_system_update_gizmo_pos_to_selection(&scene->gizmo,
                                                 &scene->selection, scene->ubo);
}

void UI::InspectorPointLight::properties_update_callback(Scene *scene,
                                                         Renderer *renderer,
                                                         void *user_data) {

  SceneEditorMeshList *list = (SceneEditorMeshList *)user_data;
  PointLight *light = (PointLight *)list->origin->target;

  ubo_update_queue_insert(scene->ubo, UBOType_LightList,
                          scene->lights.ubo_slot.id);
}

void UI::InspectorPointLight::draw() {

  ImGui::BeginChild("##Prop", ImVec2(0, 0), true);
  {

    UI::InputTextCallback<SceneEditorMeshList>(
        sem, gui, "Light name", sizeof(name_t), InputFlag_SpanFullWidth,
        sem_list_get_name, sem_list_set_name, NULL, NULL)
        .draw();

    light = (PointLight *)sem->origin->target;

    {
      // Display either shadow related transform callback or no depending on sem
      // registry type
      if (RegEntryType_SceneEditorMeshList_PointLight == type)
        inspector_tree_list_draw(sem, &transform_attributes, gui);

      else if (RegEntryType_SceneEditorMeshList_PointLightShadow == type)
        inspector_tree_list_draw(sem, &transform_shadow_attributes, gui);
    }

    ImGui::Spacing();
    inspector_tree_list_draw(light, &properties_attributes, gui);
  }
  ImGui::EndChild();
}
