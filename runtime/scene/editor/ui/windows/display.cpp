#include "display.hpp"

#include "../imgui_style/style.carbon.hpp"
#include "backend/registry.h"
#include "imgui/imgui.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/mesh/list/list.h"
#include "runtime/scene/editor/ui/components/button_icon.hpp"
#include "runtime/scene/editor/ui/components/checkbox.hpp"
#include "runtime/scene/show.h"

int UI::Display::state = DisplayState_Activity | DisplayState_Grid |
                         DisplayState_Layout | DisplayState_Light |
                         DisplayState_Probe;

void UI::Display::checkbox_update_state(bool active,
                                        const DisplayState target) {

  if (active)
    state |= target;
  else
    state &= ~target;
}

void UI::Display::checkbox_on_change_base(Scene *scene, bool active,
                                          void *user_data) {

  UI::DisplayState target = *(UI::DisplayState *)user_data;
  checkbox_update_state(active, target);
}

void UI::Display::checkbox_on_change_light(Scene *scene, bool active,
                                           void *user_data) {

  UI::DisplayState target = *(UI::DisplayState *)user_data;
  checkbox_update_state(active, target);

  static const RegEntryType light_type[7] = {
      RegEntryType_SceneEditorMeshList_AmbientLight,
      RegEntryType_SceneEditorMeshList_PointLight,
      RegEntryType_SceneEditorMeshList_SunLight,
      RegEntryType_SceneEditorMeshList_SpotLight,
      RegEntryType_SceneEditorMeshList_PointLightShadow,
      RegEntryType_SceneEditorMeshList_SunLightShadow,
      RegEntryType_SceneEditorMeshList_SpotLightShadow,
  };

  sem_list_toggle_visibility(&scene->editor.sem_list, scene, light_type,
                             sizeof(light_type) / sizeof(RegEntryType), active);
}

void UI::Display::checkbox_on_change_probe(Scene *scene, bool active,
                                           void *user_data) {

  UI::DisplayState target = *(UI::DisplayState *)user_data;
  checkbox_update_state(active, target);

  static const RegEntryType probe_type[2] = {
      RegEntryType_SceneEditorMeshList_ProbeReflectionGrid,
      RegEntryType_SceneEditorMeshList_ProbeReflectionPlane,
  };

  sem_list_toggle_visibility(&scene->editor.sem_list, scene, probe_type, 2,
                             active);
}

void UI::Display::checkbox_on_change_grid(Scene *scene, bool active,
                                          void *user_data) {

  UI::DisplayState target = *(UI::DisplayState *)user_data;
  checkbox_update_state(active, target);

  if (active)
    scene_show_mesh(scene, scene->editor.gizmo.grid);
  else
    scene_hide_mesh(scene, scene->editor.gizmo.grid);
}

void UI::Display::draw() {

  ImGui::SetNextWindowPos(ImVec2(ui->size[SceneEditorUISize_Gizmo_Margin], 0),
                          ImGuiCond_Always);
  ImGui::Begin("Display Frame", nullptr,
               ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoBackground);

  if (ImGui::BeginCombo("##Display", "Display",
                        ImGuiComboFlags_WidthFitPreview)) {
    for (int i = 0; i < filter_length; i++)
      UI::Checkbox(scene, filters[i].label, (state & filters[i].target_state),
                   filters[i].icon, filters[i].on_change,
                   (void *)&filters[i].target_state)
          .draw();

    ImGui::EndCombo();
  }

  ImGui::End();
}
