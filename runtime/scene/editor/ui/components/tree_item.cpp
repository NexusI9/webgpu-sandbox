#include "tree_item.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_wgpu.h"
#include "runtime/scene/editor/ui/components/button_icon.hpp"
#include "runtime/scene/editor/ui/core.h"
#include "runtime/scene/editor/ui/theme/theme.default.h"

bool UI::TreeItemMesh::draw() {

  // init attributes
  bool active = false;

  const ImVec2 pos = ImGui::GetCursorPos();
  const ImVec2 screen_pos = ImGui::GetCursorScreenPos();
  const float height = scene_editor_ui_size(ui, this->height);
  const float border_radius = scene_editor_ui_size(ui, this->border_radius);
  const float top_padding = scene_editor_ui_size(ui, 4.f);
  const float left_padding = scene_editor_ui_size(ui, 20.f);
  const ImVec2 dim = ImVec2(ImGui::GetContentRegionAvail().x, height);

  name_t inv_name;
  name_compose(inv_name, "##%s", label);
  name_t id_name;
  name_compose(id_name, "##tree_item_%d", id);
  name_t id_bg;
  name_compose(id_name, "##tree_item_bg_%d", id);

  ImGui::PushID(id_name);

  // === Backround Rect ===
  {
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    ImVec4 color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    ImVec4 hover_color =
        (ImVec4 &)theme_default_color[THEME_DEFAULT_COLOR_SURFACE_BASE];

    if (flag & TreeItemFlag_AltBg)
      color = (ImVec4 &)theme_default_color[THEME_DEFAULT_COLOR_SURFACE_LOW];

    // slightly shift the invisible button so it doesn't overlapp with the arrow
    // to open the tree
    ImGui::SetCursorPosX(pos.x + left_padding);

    // Need to create an invisible button that cover the background cause
    // DrawList Rect filled are not interactive elements
    ImGui::InvisibleButton(id_bg, dim);

    // set it back for the actual background color...
    ImGui::SetCursorPosX(pos.x);
    draw_list->AddRectFilled(
        screen_pos, ImVec2(screen_pos.x + dim.x, screen_pos.y + height),
        ImGui::ColorConvertFloat4ToU32(color), border_radius);

    if (ImGui::IsItemHovered())
      draw_list->AddRectFilled(
          screen_pos, ImVec2(screen_pos.x + dim.x, screen_pos.y + height),
          ImGui::ColorConvertFloat4ToU32(hover_color), border_radius);

    if (ImGui::IsItemClicked())
      clicked = true;
  }

  // === Label & Icon ===
  {
    // padding for arrow
    ImGui::SetCursorPosX(pos.x + left_padding);

    const SceneEditorUIIconUV *uv = &ui->icon_uv[icon];
    // vertical alignment
    ImGui::SetCursorPosY(pos.y + scene_editor_ui_size(ui, top_padding));
    ImGui::Image((ImTextureRef)ui->atlas_texture.view,
                 ImVec2(icon_size, icon_size), ImVec2(uv->uv0[0], uv->uv0[1]),
                 ImVec2(uv->uv1[0], uv->uv1[1]));

    ImGui::SameLine();

    ImGui::Text("%s", label);
  }

  // === Tree Overlay ===
  {
    // reset cursor to initial pos and overlay the tree on top of the label
    ImGui::SetCursorPosX(pos.x);
    ImGui::SetCursorPosY(pos.y + scene_editor_ui_size(ui, top_padding));

    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_AllowItemOverlap;

    if ((flag & TreeItemFlag_HasChild) == 0)
      flags |= ImGuiTreeNodeFlags_Leaf;

    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,
                          ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,
                          ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    active = ImGui::TreeNodeEx(inv_name, flags);

    ImGui::PopStyleColor(3);
  }

  ImGui::PopID();

  return active;
}

bool UI::TreeItemMesh::draw_visibility() {

  ImGui::SameLine(ImGui::GetWindowContentRegionMax().x -
                  scene_editor_ui_size(ui, icon_size));

  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
  ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));

  name_t button_id;
  name_compose(button_id, "button_visibility_%d", id);

  bool button = ButtonIcon(ui, SceneEditorUIIcon_Eye, button_id,
                           ImVec2(icon_size, icon_size))
                    .draw();

  ImGui::PopStyleColor(2);

  return button;
}

bool UI::TreeItem::draw() {

  ImGuiTreeNodeFlags flags =
      ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen;

  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                      ImVec2(ui->size[SceneEditorUISize_Tree_PaddingH],
                             ui->size[SceneEditorUISize_Tree_PaddingV]));
  // resolution / multisample
  bool item = ImGui::TreeNodeEx(label, flags);

  ImGui::PopStyleVar(1);

  return item;
}
