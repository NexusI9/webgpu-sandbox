#include "inspector.spot_light.hpp"
#include "backend/ubo.h"
#include "imgui/imgui.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/light/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/editor/mesh/list/list.h"
#include "runtime/scene/editor/selection/utils.h"
#include <cstdio>

void UI::InspectorSpotLight::transform_update_callback(Scene *scene,
                                                       void *user_data) {

  SceneEditorMeshList *list = (SceneEditorMeshList *)user_data;

  for (size_t i = 0; i < list->length; i++)
    ubo_update_queue_insert(scene->ubo, UBOType_Mesh,
                            list->entries[i].mesh->ubo_slot.id);

  scene_gizmo_pos_to_selection(&scene->editor.gizmo.transform,
                               &scene->editor.selection, scene->ubo);
}

void UI::InspectorSpotLight::properties_update_callback(Scene *scene,
                                                        void *user_data) {

  SceneEditorMeshList *list = (SceneEditorMeshList *)user_data;
  SpotLight *light = (SpotLight *)list->origin->target;

  ubo_update_queue_insert(scene->ubo, UBOType_LightList,
                          scene->lights.ubo_slot.id);
}

void UI::InspectorSpotLight::draw() {

  ImGui::BeginChild("##Prop", ImVec2(0, 0), true);
  {

    UI::InputTextCallback<SceneEditorMeshList>(
        sem, gui, "Light name", sizeof(name_t), InputFlag_SpanFullWidth,
        sem_list_get_name, sem_list_set_name, NULL, NULL)
        .draw();

    light = (SpotLight *)sem->origin->target;

    {
      // Display either shadow related transform callback or no depending on sem
      // registry type
      if (RegEntryType_SceneEditorMeshList_SpotLight == type)
        inspector_tree_list_draw(sem, &transform_attributes, gui);

      else if (RegEntryType_SceneEditorMeshList_SpotLightShadow == type)
        inspector_tree_list_draw(sem, &transform_shadow_attributes, gui);
    }

    ImGui::Spacing();
    inspector_tree_list_draw(light, &properties_attributes, gui);
  }
  ImGui::EndChild();
}
