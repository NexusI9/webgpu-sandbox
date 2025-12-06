#ifndef _GRADIENT_RECT_H_
#define _GRADIENT_RECT_H_

#include "../utils.hpp"
#include "utils/color.h"
#include <cglm/cglm.h>
#include <imgui/imgui.h>

namespace UI {

class GradientRect {
public:
  ImVec2 p0 = ImVec2(0, 0);
  ImVec2 p1 = ImVec2(100, 20);

  GradientRect(ImVec2 p0, ImVec2 p1, const color *colors, const int count) {
    update_position(p0, p1);
    update_colors(colors, count);
  };

  void update_position(ImVec2 p0, ImVec2 p1) {
    this->p0 = p0;
    this->p1 = p1;

    this->width = p1.x - p0.x;
    this->height = p1.y - p0.y;
  }

  // Update colors and count, and cache them internally
  void update_colors(const color *new_colors, const int new_count) {
    if (!new_colors || new_count < 2 || new_count > 32)
      return;

    count = new_count;
    for (int i = 0; i < count; i++) {
      glm_vec4_copy((float *)new_colors[i], colors[i]);
      t[i] = float(i) / float(count - 1);
    }
  }

  // Draw a horizontal gradient (left → right)
  void draw_horizontal(ImDrawList *dl) const {
    if (!dl || count < 2)
      return;

    for (int i = 0; i < count - 1; i++) {
      float t0 = t[i];
      float t1 = t[i + 1];

      ImVec2 s0(p0.x + t0 * width, p0.y);
      ImVec2 s1(p0.x + t1 * width, p1.y);

      dl->AddRectFilledMultiColor(s0, s1, im_color(colors[i]),
                                  im_color(colors[i + 1]),
                                  im_color(colors[i + 1]), im_color(colors[i]));
    }
  }

  // Draw a vertical gradient (top → bottom)
  void draw_vertical(ImDrawList *dl) const {
    if (!dl || count < 2)
      return;

    for (int i = 0; i < count - 1; i++) {
      float t0 = t[i];
      float t1 = t[i + 1];

      ImVec2 s0(p0.x, p0.y + t0 * height);
      ImVec2 s1(p1.x, p0.y + t1 * height);

      dl->AddRectFilledMultiColor(s0, s1, im_color(colors[i]),
                                  im_color(colors[i]), im_color(colors[i + 1]),
                                  im_color(colors[i + 1]));
    }
  }

private:
  static constexpr uint8_t COLOR_CAPACITY = 32;
  mutable color colors[COLOR_CAPACITY];
  mutable float t[COLOR_CAPACITY];
  mutable int count = 0;
  mutable float width, height;
};

} // namespace UI

#endif
