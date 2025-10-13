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
#include "runtime/scene/editor/ui/core.h"
#include "runtime/scene/show.h"
#include "utils/name.h"

void UI::Tree::draw_mesh(Mesh *mesh) {

  UI::TreeItem item = UI::TreeItem(
      scene, mesh->name, mesh->id, SceneEditorUIIcon_Mesh,
      ui->size[SceneEditorUISize_Button_InspectorTab], mesh->children.length);

  item.draw_label();

  if (ImGui::IsItemClicked())
    scene_selection_toggle_mesh(scene, mesh);

  if (ImGui::IsItemToggledOpen())
    for (size_t i = 0; i < mesh->children.length; i++)
      draw_mesh(mesh->children.entries[i]);

  item.draw_visibility();

  if (ImGui::IsItemClicked())
    scene_visibility_toggle_mesh(scene, mesh);
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
                                           const RegEntryType type) {

  // define icon based on SEM type
  SceneEditorUIIcon icon = sem_icon_map[type];

  UI::TreeItem item =
      UI::TreeItem(scene, list->name, list->id, icon,
                   ui->size[SceneEditorUISize_Button_InspectorTab], false);

  item.draw_label();

  if (ImGui::IsItemClicked())
    scene_selection_toggle_mesh(scene, list->entries->mesh);

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
    for (size_t i = 0; i < scene->editor.ui.tree.length; i++) {

      const RegEntry *entry = reg_lookup(scene->editor.ui.tree.entries[i]);

      switch (entry->type) {

      case RegEntryType_Mesh:
        draw_mesh((Mesh *)entry->ptr);
        break;

      case RegEntryType_SceneEditorMeshList_AmbientLight:
      case RegEntryType_SceneEditorMeshList_PointLight:
      case RegEntryType_SceneEditorMeshList_SunLight:
      case RegEntryType_SceneEditorMeshList_SpotLight:
      case RegEntryType_SceneEditorMeshList_ProbeReflectionGrid:
      case RegEntryType_SceneEditorMeshList_ProbeReflectionPlane:
        draw_scene_editor_mesh_list((SceneEditorMeshList *)entry->ptr,
                                    entry->type);
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

bool UI::TreeItem::draw_label() {

  // ImGuiTreeNodeFlags_SpanAvailWidth
  ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                             ImGuiTreeNodeFlags_AllowItemOverlap |
                             ImGuiTreeNodeFlags_FramePadding;

  if (has_child == 0)
    flags |= ImGuiTreeNodeFlags_Leaf;

  // Tree item
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                      ImVec2(ui->size[SceneEditorUISize_Tree_PaddingH],
                             ui->size[SceneEditorUISize_Tree_PaddingV]));

  name_t inv_name;
  name_compose(inv_name, "##%s", label);

  bool tree_item = ImGui::TreeNodeEx(inv_name, flags);

  ImGui::SameLine();

  const SceneEditorUIIconUV *uv = &ui->icon_uv[icon];
  ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8); // alignment
  ImGui::Image((ImTextureRef)ui->atlas_texture.view,
               ImVec2(icon_size, icon_size), ImVec2(uv->uv0[0], uv->uv0[1]),
               ImVec2(uv->uv1[0], uv->uv1[1]));

  ImGui::SameLine();
  ImGui::Text("%s", label);

  if (tree_item) {
    ImGui::TreePop();
  }
  ImGui::PopStyleVar();

  return tree_item;
}

bool UI::TreeItem::draw_visibility() {

  ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - ui->dpi * icon_size);

  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

  name_t button_id;
  name_compose(button_id, "button_visibility_%d", id);

  bool button = ButtonIcon(ui, SceneEditorUIIcon_Eye, button_id,
                           ImVec2(icon_size, icon_size))
                    .draw();

  ImGui::PopStyleColor(1);

  return button;
}
