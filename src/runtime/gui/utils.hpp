#ifndef _GUI_UTILS_HPP_
#define _GUI_UTILS_HPP_

#include "./core.h"
#include "imgui/imgui_impl_wgpu.h"
#include "utils/color.h"
#include "utils/defines.h"
#include "webgpu/webgpu.h"
#include <cglm/cglm.h>
#include <imgui/imgui.h>

EXTERN_C_BEGIN

static inline ImColor im_color(color col) {
  return ImColor(col[0], col[1], col[2], col[3]);
}

static inline ImVec2 im_vec2(vec2 val) { return ImVec2(val[0], val[1]); }

static inline void glm_vec2(ImVec2 val, vec2 dest) {
  glm_vec2_copy((vec2){val.x, val.y}, dest);
}

static inline ImColor im_color_lerp(const ImColor start, const ImColor end,
                                    const float t) {
  const float inv_t = 1.0f - t;

  return ImColor(start.Value.x * inv_t + end.Value.x * t, // R
                 start.Value.y * inv_t + end.Value.y * t, // G
                 start.Value.z * inv_t + end.Value.z * t, // B
                 start.Value.w * inv_t + end.Value.w * t  // A
  );
}

static inline ImVec2 gui_scale_im_vec2(const Gui *gui, const ImVec2 value) {
  return ImVec2(gui_scale(gui, value.x), gui_scale(gui, value.y));
}

EXTERN_C_END

#endif
