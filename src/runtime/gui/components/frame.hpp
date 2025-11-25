#ifndef _WIDGET_FRAME_H_
#define _WIDGET_FRAME_H_

#include <imgui/imgui.h>

namespace UI {

class Frame {
public:
  ImVec2 position = ImVec2(0, 0);
  ImVec2 size = ImVec2(0, 0); // 0 = auto size to content

  ImVec2 padding = ImVec2(6, 6);

  ImU32 background_color = IM_COL32(30, 30, 30, 255);
  ImU32 border_color = IM_COL32(255, 255, 255, 255);
  float border_thickness = 1.0f;
  float border_radius = 4.0f;

public:
  // Start the frame
  void Begin(const char *id) {
    ImGui::PushID(id);

    // Prepare absolute position if set
    ImGui::SetCursorScreenPos(position);

    // Save where the frame begins
    startPos = ImGui::GetCursorScreenPos();

    // Split channels:
    // 0 = background, 1 = content
    drawList = ImGui::GetWindowDrawList();
    drawList->ChannelsSplit(2);

    // Switch to content channel
    drawList->ChannelsSetCurrent(1);

    // Begin content measurement
    ImGui::BeginGroup();
  }

  // End the frame (draw background + border)
  void End() {
    ImGui::EndGroup();

    // Get content bounding rect
    ImVec2 contentMin = ImGui::GetItemRectMin();
    ImVec2 contentMax = ImGui::GetItemRectMax();

    // Resolve final size
    ImVec2 finalMin = startPos;
    ImVec2 finalMax;

    if (size.x == 0)
      finalMax.x = contentMax.x + padding.x;
    else
      finalMax.x = finalMin.x + size.x;

    finalMin.x -= padding.x;

    if (size.y == 0)
      finalMax.y = contentMax.y + padding.y;
    else
      finalMax.y = finalMin.y + size.y;

    finalMin.y -= padding.y;

    // Draw background in channel 0
    drawList->ChannelsSetCurrent(0);
    drawList->AddRectFilled(finalMin, finalMax, background_color,
                            border_radius);

    // Draw border
    if (border_thickness > 0.0f) {
      drawList->AddRect(finalMin, finalMax, border_color, border_radius, 0,
                        border_thickness);
    }

    // Back to normal drawing
    drawList->ChannelsMerge();
    ImGui::PopID();
  }

private:
  ImDrawList *drawList = nullptr;
  ImVec2 startPos;
};
} // namespace UI

#endif
