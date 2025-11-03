#include "button_icon.hpp"
#include "backend/theme/core.h"

bool UI::ButtonIcon::draw() {

  const ThemeIconCell *uv = theme_icon_cell(gui->theme, icon);
  return ImGui::ImageButton(id, (ImTextureRef)theme_icon_atlas(gui->theme),
                            size, ImVec2(uv->uv0[0], uv->uv0[1]),
                            ImVec2(uv->uv1[0], uv->uv1[1]));
}
