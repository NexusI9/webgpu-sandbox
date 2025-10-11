#include "button_group.hpp"
#include "imgui/imgui.h"
#include "utils/name.h"

void UI::ButtonGroup::draw() {

  ImGui::Begin(label, nullptr,
               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_NoBackground);

  ImVec2 pos = ImGui::GetCursorScreenPos();
  ImDrawList *draw_list = ImGui::GetWindowDrawList();

  for (int i = 0; i < items_length; i++) {

    ImVec2 btn_pos = ImVec2(pos.x, pos.y + i * style->size);
    {
      ImGui::SetCursorScreenPos(btn_pos);
      name_t button_id;
      name_compose(button_id, "button%s%d", label, i);
      ImGui::PushID(button_id);
      ImGui::InvisibleButton("button_group_item",
                             ImVec2(style->size, style->size));
    }

    ImDrawFlags flags = 0;
    float radius = 0.0f;

    if (i == 0) {
      radius = style->border_radius;
      flags |=
          ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight;
    }

    else if (i == items_length - 1) {
      radius = style->border_radius;
      flags |= ImDrawFlags_RoundCornersBottomLeft |
               ImDrawFlags_RoundCornersBottomRight;
    }

    draw_list->AddRectFilled(
        ImVec2(pos.x, pos.y + i * style->size),
        ImVec2(pos.x + style->size, pos.y + (i + 1) * style->size),
        ImGui::ColorConvertFloat4ToU32(style->background_default),
        radius, flags);

    bool hovered = ImGui::IsItemHovered();
    bool clicked = ImGui::IsItemClicked();

    if (hovered)
      draw_list->AddRectFilled(
          ImVec2(pos.x, pos.y + i * style->size),
          ImVec2(pos.x + style->size, pos.y + (i + 1) * style->size),
          ImGui::ColorConvertFloat4ToU32(style->background_hover),
          radius, flags);

    ImVec2 imgMin = ImVec2(btn_pos.x + style->inner_padding,
                           btn_pos.y + style->inner_padding);

    ImVec2 imgMax = ImVec2(btn_pos.x + style->size - style->inner_padding,
                           btn_pos.y + style->size - style->inner_padding);

    SceneEditorUIIconUV *uv = &ui->icon_uv[items[i].icon];
    draw_list->AddImage((ImTextureRef)ui->atlas_texture.view, imgMin, imgMax,
                        ImVec2(uv->uv0[0], uv->uv0[1]),
                        ImVec2(uv->uv1[0], uv->uv1[1]));

    if (clicked && items[i].on_click_callback)
      items[i].on_click_callback(scene);

    ImGui::PopID();
  }

  ImGui::SetCursorScreenPos(
      ImVec2(pos.x, pos.y + style->size + style->inner_padding));

  ImGui::End();
}
