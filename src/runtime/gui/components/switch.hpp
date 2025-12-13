#ifndef _COMPONENT_SWITCH_H_
#define _COMPONENT_SWITCH_H_

#include <imgui/imgui.h>

namespace UI {

class Switch {
public:
  // Switch style
  float width = 50.0f;
  float height = 25.0f;
  float border_radius = 12.0f;

  ImU32 bg_color_default = IM_COL32(180, 180, 180, 255);
  ImU32 bg_color_active = IM_COL32(0, 150, 250, 255);

  ImU32 border_color_default = IM_COL32(100, 100, 100, 255);
  ImU32 border_color_active = IM_COL32(0, 100, 200, 255);

  float border_width = 1.0f;

  ImU32 dot_color_default = IM_COL32(255, 255, 255, 255);
  ImU32 dot_color_active = IM_COL32(255, 255, 255, 255);

  float dot_border_radius = 8.0f;
  float dot_size = 25.0f;
  const char *id;

private:
  bool enabled = false;

public:
  // Constructor (optional initial state)
  Switch(bool initial = false) : enabled(initial) {}

  // Logic part only
  bool Update(const char *label = nullptr) {
    // Reserve space for the switch
    ImGui::InvisibleButton(label ? label : "##switch", ImVec2(width, height));
    bool clicked = ImGui::IsItemClicked();

    if (clicked)
      enabled = !enabled;

    return enabled;
  }

  // Visual only (no logic, use Draw() for logic + visual)
  bool Render() {
    ImVec2 pos = ImGui::GetCursorScreenPos();

    // Use Dummy as "boundbox", useful if we don't render direcly after update
    ImGui::SetCursorPos(pos);
    ImGui::Dummy(ImVec2(width, height));

    // Colors depending on state
    ImU32 bg_color = enabled ? bg_color_active : bg_color_default;
    ImU32 border_color = enabled ? border_color_active : border_color_default;
    ImU32 dot_color = enabled ? dot_color_active : dot_color_default;

    // Draw background rounded rect
    ImDrawList *dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), bg_color,
                      border_radius);
    if (border_width > 0.0f)
      dl->AddRect(pos, ImVec2(pos.x + width, pos.y + height), border_color,
                  border_radius, 0, border_width);

    // Draw the dot
    ImVec2 dot_min =
        ImVec2(pos.x + (enabled ? width - dot_size - (height - dot_size) * 0.5f
                                : (height - dot_size) * 0.5f),
               pos.y + (height - dot_size) * 0.5f);
    ImVec2 dot_max = ImVec2(dot_min.x + dot_size, dot_min.y + dot_size);

    dl->AddRectFilled(dot_min, dot_max, dot_color, dot_border_radius);

    return enabled;
  }

  // Update (logic) + Render (visual)
  bool Draw(const char *label) {
    Update(label);
    return Render();
  }

  // Optionally get/set state
  bool IsEnabled() const { return enabled; }
  void SetEnabled(bool value) { enabled = value; }
};

} // namespace UI

#endif
