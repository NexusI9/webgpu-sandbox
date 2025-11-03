#include "combobox_resolution.hpp"
#include "runtime/gui/components/input.hpp"

bool UI::ComboboxResolution::draw() {
  
  UI::Combobox point_res = UI::Combobox(gui, label, style, default_value);

  if (point_res.draw()) {

    for (int i = 0; i < fixed_resolution_count; ++i) {

      const bool is_selected =
          (fixed_resolutions[i].resolution == current_resolution);

      if (ImGui::Selectable(fixed_resolutions[i].label, is_selected))
        on_select_callback(scene, fixed_resolutions[i].resolution);

      // Set the initial focus when opening the combo (for keyboard
      // navigation)
      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }

    point_res.end();
  }

  return false;
}
