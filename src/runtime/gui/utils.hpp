#ifndef _GUI_UTILS_HPP_
#define _GUI_UTILS_HPP_

#include "utils/color.h"
#include "utils/defines.h"
#include <imgui/imgui.h>

EXTERN_C_BEGIN

static inline ImColor im_color(color col) {
  return ImColor(col[0], col[1], col[2], col[3]);
}

static inline ImVec2 im_vec2(vec2 val) { return ImVec2(val[0], val[1]); }

EXTERN_C_END

#endif
