#ifndef _GUI_DRAW_HPP_
#define _GUI_DRAW_HPP_

#include "./core.h"
#include "backend/context.h"
#include "imgui/imgui.h"
#include "runtime/input/core.h"

// Use a different hpp file to well segment the C from C++, prenventing
// polluting
static inline void gui_draw_update_io(Gui *gui) {

  ImGuiIO &io = ImGui::GetIO();
  io.DisplaySize.x = gui_scale(gui, context_width());
  io.DisplaySize.y = gui_scale(gui, context_height());
  io.DeltaTime = g_clock.delta;
  io.FontGlobalScale = gui->dpi;
  io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
  io.MousePos =
      ImVec2(gui_scale(gui, g_input.mouse.x), gui_scale(gui, g_input.mouse.y));

  io.MouseDown[ImGuiMouseButton_Left] =
      g_input.mouse.state[InputMouseButton_Left];

  io.MouseDown[ImGuiMouseButton_Middle] =
      g_input.mouse.state[InputMouseButton_Middle];

  io.MouseDown[ImGuiMouseButton_Right] =
      g_input.mouse.state[InputMouseButton_Right];

  io.MouseWheel = g_input.mouse.wheel.deltaY;
  io.MouseWheelH = g_input.mouse.wheel.deltaX;

  if (io.WantCaptureMouse)
    g_input.locked |= InputLockState_Mouse;
  else if (g_input.locked & InputLockState_Mouse)
    g_input.locked ^= InputLockState_Mouse;
}

#endif
