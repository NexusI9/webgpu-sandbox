#ifndef _COMPONENT_SPRITE_H_
#define _COMPONENT_SPRITE_H_

#include "../sprite.h"
#include "../utils.hpp"
#include "runtime/texture/atlas.h"
#include <imgui/imgui.h>
#include <webgpu/webgpu.h>

namespace Component {

class Sprite {

public:
  Sprite(WGPUTextureView view, const TextureAtlasRegion *region)
      : view(view), region(region) {}

  void set_position(ImVec2 pos, const GuiSpriteAnchor anchor) {

    vec2 glm_pos;
    glm_vec2(pos, glm_pos);

    vec2 start, end;
    gui_sprite_set_endpoints(glm_pos, region->size, anchor, start, end);

    this->start[0] = start[0], this->start[1] = start[1];
    this->end[0] = end[0], this->end[1] = end[1];
  }

  void set_position_x(float pos, const GuiSpriteAnchor anchor) {
    set_position(ImVec2(pos, start.y), anchor);
  }

  void set_position_y(float pos, const GuiSpriteAnchor anchor) {
    set_position(ImVec2(start.x, pos), anchor);
  }

  void draw() {
    ImDrawList *dl = ImGui::GetWindowDrawList();
    dl->AddImage((ImTextureRef)view, start, end, im_vec2((float *)region->uv0),
                 im_vec2((float *)region->uv1));
  }

  void draw_at(ImVec2 offset) {
    ImDrawList *dl = ImGui::GetWindowDrawList();
    dl->AddImage((ImTextureRef)view,
                 ImVec2(start.x + offset.x, start.y + offset.y),
                 ImVec2(end.x + offset.x, end.y + offset.y),
                 im_vec2((float *)region->uv0), im_vec2((float *)region->uv1));
  }

  bool clicked(const ImGuiMouseButton button) {
    if (ImGui::IsMouseClicked(button) && hovered())
      return true;

    return false;
  }

  bool hovered() {
    if (ImGui::IsMouseHoveringRect(start, end))
      return true;

    return false;
  }

  const TextureAtlasRegion *region;
  const ImVec2 get_start() { return start; }
  const ImVec2 get_end() { return end; }
  const WGPUTextureView get_view() { return view; }

private:
  WGPUTextureView view;
  ImVec2 start, end;
};

} // namespace Component

#endif
