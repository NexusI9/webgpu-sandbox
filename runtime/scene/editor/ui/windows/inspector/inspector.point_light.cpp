#include "inspector.point_light.hpp"
#include "backend/registry.h"
#include "backend/ubo.h"
#include "imgui/imgui.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/light/core.h"
#include "runtime/mesh/transform.h"
#include "runtime/scene/editor/mesh/list/list.h"
#include "runtime/scene/editor/selection/utils.h"
#include <cstdio>

void UI::InspectorPointLight::transform_update_callback(Scene *scene,
                                                        void *user_data) {

  SceneEditorMeshList *list = (SceneEditorMeshList *)user_data;

  for (size_t i = 0; i < list->length; i++)
    ubo_update_queue_insert(&scene->renderer.ubo, UBOType_Mesh,
                            list->entries[i].mesh->ubo_slot.id);

  scene_gizmo_pos_to_selection(&scene->editor.gizmo.transform,
                               &scene->editor.selection, &scene->renderer.ubo);
}

void UI::InspectorPointLight::properties_update_callback(Scene *scene,
                                                         void *user_data) {

  SceneEditorMeshList *list = (SceneEditorMeshList *)user_data;
  PointLight *light = (PointLight *)list->origin->target;

  ubo_update_queue_insert(&scene->renderer.ubo, UBOType_LightList,
                          scene->lights.ubo_slot.id);
}

void UI::InspectorPointLight::draw() {

  ImGui::BeginChild("##Prop", ImVec2(0, 0), true);
  {

    UI::InputTextCallback<SceneEditorMeshList>(
        sem, scene, "Light name", sizeof(name_t), InputFlag_SpanFullWidth,
        sem_list_get_name, sem_list_set_name, NULL, NULL)
        .draw();

    light = (PointLight *)sem->origin->target;

    {
      // Display either shadow related transform callback or no depending on sem
      // registry type
      if (RegEntryType_SceneEditorMeshList_PointLight == type)
        inspector_tree_list_draw(sem, &transform_attributes, scene);

      else if (RegEntryType_SceneEditorMeshList_PointLightShadow == type)
        inspector_tree_list_draw(sem, &transform_shadow_attributes, scene);
    }

    ImGui::Spacing();
    inspector_tree_list_draw(light, &properties_attributes, scene);
  }
  ImGui::EndChild();
}
