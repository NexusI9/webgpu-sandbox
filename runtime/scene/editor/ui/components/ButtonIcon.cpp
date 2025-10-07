#include "ButtonIcon.hpp"

bool UI::ButtonIcon::draw() {

  SceneEditorUIIconUV *uv = &ui->icon_uv[icon];
  return ImGui::ImageButton(id, (ImTextureRef)ui->atlas_texture.view, size,
                            ImVec2(uv->uv0[0], uv->uv0[1]),
                            ImVec2(uv->uv1[0], uv->uv1[1]));
}
