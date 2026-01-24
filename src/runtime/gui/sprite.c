#include "sprite.h"

#include <cglm/cglm.h>

void gui_sprite_set_endpoints(const vec2 position, const vec2 size,
                              const GuiSpriteAnchor anchor, vec2 p0, vec2 p1) {
  float w = size[0];
  float h = size[1];

  float x = position[0];
  float y = position[1];

  float left, right, top, bottom;

  switch (anchor) {

  case GuiSpriteAnchor_TopLeft:
    left = x;
    top = y;
    break;

  case GuiSpriteAnchor_TopMiddle:
    left = x - w * 0.5f;
    top = y;
    break;

  case GuiSpriteAnchor_TopRight:
    left = x - w;
    top = y;
    break;

  case GuiSpriteAnchor_MiddleLeft:
    left = x;
    top = y - h * 0.5f;
    break;

  case GuiSpriteAnchor_Center:
    left = x - w * 0.5f;
    top = y - h * 0.5f;
    break;

  case GuiSpriteAnchor_MiddleRight:
    left = x - w;
    top = y - h * 0.5f;
    break;

  case GuiSpriteAnchor_BottomLeft:
    left = x;
    top = y - h;
    break;

  case GuiSpriteAnchor_BottomMiddle:
    left = x - w * 0.5f;
    top = y - h;
    break;

  case GuiSpriteAnchor_BottomRight:
    left = x - w;
    top = y - h;
    break;
  }

  right = left + w;
  bottom = top + h;

  glm_vec2_copy((vec2){left, top}, p0);
  glm_vec2_copy((vec2){right, bottom}, p1);
}
