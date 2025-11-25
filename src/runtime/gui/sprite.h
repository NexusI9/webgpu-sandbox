#ifndef _GUI_SPRITE_H_
#define _GUI_SPRITE_H_

#include "utils/defines.h"
#include <cglm/cglm.h>

typedef enum {
  GuiSpriteAnchor_TopLeft,
  GuiSpriteAnchor_TopMiddle,
  GuiSpriteAnchor_RightMiddle,
  GuiSpriteAnchor_MiddleLeft,
  GuiSpriteAnchor_Center,
  GuiSpriteAnchor_MiddleRight,
  GuiSpriteAnchor_BottomLeft,
  GuiSpriteAnchor_BottomMiddle,
  GuiSpriteAnchor_BottomRight,
} GuiSpriteAnchor;

EXTERN_C_BEGIN

void gui_sprite_set_endpoints(const vec2, const vec2, const GuiSpriteAnchor,
                              vec2, vec2);

EXTERN_C_END

#endif
