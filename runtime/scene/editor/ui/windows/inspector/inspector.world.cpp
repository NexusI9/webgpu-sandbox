#include "inspector.world.hpp"
#include "imgui/imgui.h"
#include "runtime/scene/editor/ui/components/tree_item.hpp"
#include "webgpu/webgpu.h"

void UI::WorldTab::draw() {

  if (UI::TreeItem(scene, "Skybox").draw()) {

    
    
    ImGui::TreePop();
  }

  if (UI::TreeItem(scene, "Shadow").draw()) {

    //wgpuTextureGetWidth();
    
    ImGui::TreePop();
  }

  if (UI::TreeItem(scene, "Reflection").draw()) {

    ImGui::TreePop();
  }
}
