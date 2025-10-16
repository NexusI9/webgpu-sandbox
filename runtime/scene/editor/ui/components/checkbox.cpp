#include "checkbox.hpp"
#include "imgui/imgui.h"
#include "runtime/scene/editor/ui/core.h"

bool UI::Checkbox::draw() {

  ImGui::PushID(label);

  bool checkbox = ImGui::Checkbox("##checkbox", &active);
  
  if (checkbox) {
    
    if (on_change)
      on_change(scene, active, user_data);
  }

  ImGui::SameLine(ImGui::GetCursorPosX() + 60);

  SceneEditorUIIconUV *uv = &ui->icon_uv[icon];
  float size = ui->size[SceneEditorUISize_Button_InspectorTab];

  ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 8);

  ImGui::Image((ImTextureRef)ui->atlas_texture.view, ImVec2(size, size),
               ImVec2(uv->uv0[0], uv->uv0[1]), ImVec2(uv->uv1[0], uv->uv1[1]));

  ImGui::SameLine(ImGui::GetCursorPosX() + size + 80);

  ImGui::Text("%s", label);
  ImGui::PopID();

  return checkbox;
}
