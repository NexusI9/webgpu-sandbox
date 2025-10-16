#include "tree.hpp"
#include "backend/logger.h"
#include "backend/registry.h"
#include "imgui/imgui.h"
#include "runtime/light/core.h"
#include "runtime/mesh/core.h"
#include "runtime/probe/reflection/grid.h"
#include "runtime/probe/reflection/plane.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor/selection/core.h"
#include "runtime/scene/editor/ui/components/button_icon.hpp"
#include "runtime/scene/editor/ui/components/tree_item.hpp"
#include "runtime/scene/editor/ui/core.h"
#include "runtime/scene/show.h"
#include "utils/name.h"

void UI::Tree::draw_mesh(Mesh *mesh, const size_t index) {

  int flag = TreeItemFlag_None;

  if (index % 2 == 0)
    flag |= TreeItemFlag_AltBg;

  if (mesh->children.length)
    flag |= TreeItemFlag_HasChild;

  UI::TreeItemMesh item = UI::TreeItemMesh(
      scene, mesh->name, mesh->id, SceneEditorUIIcon_Mesh,
      ui->size[SceneEditorUISize_Button_InspectorTab], (TreeItemFlag)flag);

  bool draw_label = item.draw();

  if (item.clicked){
    scene_selection_toggle_mesh(scene, mesh);
    item.close_click();
  }

  {
    item.draw_visibility();
    if (ImGui::IsItemClicked())
      scene_visibility_toggle_mesh(scene, mesh);
  }

  if (draw_label) {
    for (size_t i = 0; i < mesh->children.length; i++)
      draw_mesh(mesh->children.entries[i], i);
    ImGui::TreePop();
  }
}

static const SceneEditorUIIcon sem_icon_map[SCENE_EDITOR_UI_ICON_COUNT] = {
    [RegEntryType_SceneEditorMeshList_AmbientLight] =
        SceneEditorUIIcon_AmbientLight,
    [RegEntryType_SceneEditorMeshList_PointLight] =
        SceneEditorUIIcon_PointLight,
    [RegEntryType_SceneEditorMeshList_SunLight] = SceneEditorUIIcon_SunLight,
    [RegEntryType_SceneEditorMeshList_SpotLight] = SceneEditorUIIcon_SpotLight,
    [RegEntryType_SceneEditorMeshList_ProbeReflectionGrid] =
        SceneEditorUIIcon_ProbeReflectionGrid,
    [RegEntryType_SceneEditorMeshList_ProbeReflectionPlane] =
        SceneEditorUIIcon_ProbeReflectionPlane,
};

void UI::Tree::draw_scene_editor_mesh_list(SceneEditorMeshList *list,
                                           const RegEntryType type,
                                           const size_t index) {

  // define icon based on SEM type
  SceneEditorUIIcon icon = sem_icon_map[type];

  UI::TreeItemMesh item =
      UI::TreeItemMesh(scene, list->name, list->id, icon,
                       ui->size[SceneEditorUISize_Button_InspectorTab],
                       index % 2 == 0 ? TreeItemFlag_AltBg : TreeItemFlag_None);

  if (item.draw()) {
    ImGui::TreePop();
  }

  if (item.clicked){
    scene_selection_toggle_mesh(scene, list->entries->mesh);
    item.close_click();
  }

  item.draw_visibility();

  if (ImGui::IsItemClicked())
    for (size_t i = 0; i < list->length; i++)
      scene_visibility_toggle_mesh(scene, list->entries[i].mesh);
}

void UI::Tree::draw() {

  ImGui::BeginChild("Tree", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.3f),
                    true);
  ImGui::Text("Scene Tree");

  ImGui::BeginChild("Tree items", ImVec2(0, 0), true);
  {
    size_t index = 0;
    for (size_t i = 0; i < scene->editor.ui.tree.length; i++) {

      const RegEntry *entry = reg_lookup(scene->editor.ui.tree.entries[i]);

      switch (entry->type) {

      case RegEntryType_Mesh:
        draw_mesh((Mesh *)entry->ptr, index++);
        break;

      case RegEntryType_SceneEditorMeshList_AmbientLight:
      case RegEntryType_SceneEditorMeshList_PointLight:
      case RegEntryType_SceneEditorMeshList_SunLight:
      case RegEntryType_SceneEditorMeshList_SpotLight:
      case RegEntryType_SceneEditorMeshList_ProbeReflectionGrid:
      case RegEntryType_SceneEditorMeshList_ProbeReflectionPlane:
        draw_scene_editor_mesh_list((SceneEditorMeshList *)entry->ptr,
                                    entry->type, index++);
        break;

      default:
        // unhandeld type
        break;
      }
    }
  }
  ImGui::EndChild();
  ImGui::EndChild();
}
