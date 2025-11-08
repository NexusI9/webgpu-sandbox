#ifndef _GUI_INPUT_H_
#define _GUI_INPUT_H_

#include "backend/ubo.h"
#include "imgui/imgui.h"
#include "runtime/gui/components/core.hpp"
#include "runtime/gui/core.h"
#include "runtime/input/core.h"
#include "utils/color.h"
#include "utils/name.h"
#include <cstdio>

namespace UI {

typedef enum {
  InputFlag_None = 0,
  InputFlag_SpanFullWidth = 1 << 0,
} InputFlag;

static inline void input_style_begin(Gui *gui) {
  ImGui::PushStyleColor(
      ImGuiCol_FrameBg,
      (ImVec4 &)*theme_color(gui->theme, ThemeColor_Surface_Low));

  ImGui::PushStyleColor(
      ImGuiCol_FrameBgHovered,
      (ImVec4 &)*theme_color(gui->theme, ThemeColor_Surface_Base));

  ImGui::PushStyleColor(
      ImGuiCol_FrameBgActive,
      (ImVec4 &)*theme_color(gui->theme, ThemeColor_Surface_Base));
}

static inline void input_style_end() { ImGui::PopStyleColor(3); }
/**
                        ▗▄▄▄ ▗▄▄▄▖▗▄▄▖ ▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖
                        ▐▌  █  █  ▐▌ ▐▌▐▌   ▐▌     █
                        ▐▌  █  █  ▐▛▀▚▖▐▛▀▀▘▐▌     █
                        ▐▙▄▄▀▗▄█▄▖▐▌ ▐▌▐▙▄▄▖▝▚▄▄▖  █

                        ▗▄▄▄▖▗▖  ▗▖▗▄▄▖ ▗▖ ▗▖▗▄▄▄▖▗▄▄▖
                          █  ▐▛▚▖▐▌▐▌ ▐▌▐▌ ▐▌  █ ▐▌
                          █  ▐▌ ▝▜▌▐▛▀▘ ▐▌ ▐▌  █  ▝▀▚▖
                        ▗▄█▄▖▐▌  ▐▌▐▌   ▝▚▄▞▘  █ ▗▄▄▞▘

   Basic inputs with few style and layout options (direction/ spacing).
   Ideal for general use case.

 */

typedef enum {
  InputDirection_Vertical,
  InputDirection_Horizontal,
} InputDirection;

typedef struct {
  const InputDirection direction;
  const int label_width;
} InputStyle;

// ===== Base =====
class InputBase : public Component {
public:
  InputBase(Gui *gui, const char *label, const InputStyle *style)
      : Component(gui, label), style(style) {}

protected:
  const InputStyle *style;
};

class DragInt : public InputBase {
public:
  DragInt(Gui *gui, const char *label, const InputStyle *style, int *value,
          float speed = 1.0f, int min = 0, int max = 0,
          const char *format = "%d", ImGuiSliderFlags flags = 0)
      : InputBase(gui, label, style), value(value), speed(speed), min(min),
        max(max), format(format), flags(flags) {}

  bool draw() override {
    ImGui::Text("%s", label);
    if (style->direction == InputDirection_Horizontal)
      ImGui::SameLine(style->label_width);

    name_t input_label;
    name_compose(input_label, "##%s", label);

    input_style_begin(gui);
    bool changed =
        ImGui::DragInt(input_label, value, speed, min, max, format, flags);
    input_style_end();
    return changed;
  }

private:
  int *value;
  float speed;
  int min;
  int max;
  const char *format;
  ImGuiSliderFlags flags;
};

// ===== Float =====
class DragFloat : public InputBase {
public:
  DragFloat(Gui *gui, const char *label, const InputStyle *style, float *value,
            float speed = 0.1f, float min = 0.0f, float max = 0.0f,
            const char *format = "%.3f", ImGuiSliderFlags flags = 0)
      : InputBase(gui, label, style), value(value), speed(speed), min(min),
        max(max), format(format), flags(flags) {}

  bool draw() override {

    ImGui::Text("%s", label);
    if (style->direction == InputDirection_Horizontal)
      ImGui::SameLine(style->label_width);

    name_t input_label;
    name_compose(input_label, "##%s", label);

    input_style_begin(gui);
    bool changed =
        ImGui::DragFloat(input_label, value, speed, min, max, format, flags);
    input_style_end();
    return changed;
  }

private:
  float *value;
  float speed;
  float min;
  float max;
  const char *format;
  ImGuiSliderFlags flags;
};

// ===== Float3 =====
class DragFloat3 : public InputBase {
public:
  DragFloat3(Gui *gui, const char *label, const InputStyle *style,
             float *values, float speed = 0.1f, float min = 0.0f,
             float max = 0.0f, const char *format = "%.3f",
             ImGuiSliderFlags flags = 0)
      : InputBase(gui, label, style), values(values), speed(speed), min(min),
        max(max), format(format), flags(flags) {}

  bool draw() override {

    ImGui::Text("%s", label);
    if (style->direction == InputDirection_Horizontal)
      ImGui::SameLine(style->label_width);

    name_t input_label;
    name_compose(input_label, "##%s", label);

    input_style_begin(gui);
    bool changed =
        ImGui::DragFloat3(input_label, values, speed, min, max, format, flags);
    input_style_end();
    return changed;
  }

private:
  float *values;
  float speed;
  float min;
  float max;
  const char *format;
  ImGuiSliderFlags flags;
};
 
// ===== Float4 =====
class DragFloat4 : public InputBase {
public:
  DragFloat4(Gui *gui, const char *label, const InputStyle *style,
             float *values, float speed = 0.1f, float min = 0.0f,
             float max = 0.0f, const char *format = "%.3f",
             ImGuiSliderFlags flags = 0)
      : InputBase(gui, label, style), values(values), speed(speed), min(min),
        max(max), format(format), flags(flags) {}

  bool draw() override {

    ImGui::Text("%s", label);
    if (style->direction == InputDirection_Horizontal)
      ImGui::SameLine(style->label_width);

    name_t input_label;
    name_compose(input_label, "##%s", label);

    input_style_begin(gui);
    bool changed =
        ImGui::DragFloat4(input_label, values, speed, min, max, format, flags);
    input_style_end();
    return changed;
  }

private:
  float *values;
  float speed;
  float min;
  float max;
  const char *format;
  ImGuiSliderFlags flags;
};

// ===== Combobox =====
class Combobox : public InputBase {
public:
  Combobox(Gui *gui, const char *label, const InputStyle *style,
           const char *value, ImGuiComboFlags flags = 0)
      : InputBase(gui, label, style), value(value), flags(flags) {}

  bool draw() override {

    ImGui::Text("%s", label);
    if (style->direction == InputDirection_Horizontal)
      ImGui::SameLine(style->label_width);

    name_t input_label;
    name_compose(input_label, "##%s", label);

    input_style_begin(gui);
    bool changed = ImGui::BeginCombo("##Multisample", value, flags);
    input_style_end();
    return changed;
  }

  void end() { ImGui::EndCombo(); }

private:
  const char *value;
  ImGuiComboFlags flags;
};

/**
     ▗▄▖  ▗▄▄▖ ▗▄▄▖▗▄▄▄▖ ▗▄▄▖ ▗▄▄▖     ▄  ▗▖  ▗▖▗▖ ▗▖▗▄▄▄▖▗▄▖▗▄▄▄▖▗▄▄▄▖
    ▐▌ ▐▌▐▌   ▐▌   ▐▌   ▐▌   ▐▌       █   ▐▛▚▞▜▌▐▌ ▐▌  █ ▐▌ ▐▌ █  ▐▌
    ▐▛▀▜▌▐▌   ▐▌   ▐▛▀▀▘ ▝▀▚▖ ▝▀▚▖   █    ▐▌  ▐▌▐▌ ▐▌  █ ▐▛▀▜▌ █  ▐▛▀▀▘
    ▐▌ ▐▌▝▚▄▄▖▝▚▄▄▖▐▙▄▄▖▗▄▄▞▘▗▄▄▞▘  █     ▐▌  ▐▌▝▚▄▞▘  █ ▐▌ ▐▌ █  ▐▙▄▄▖

                     ▗▄▄▄▖▗▖  ▗▖▗▄▄▖ ▗▖ ▗▖▗▄▄▄▖▗▄▄▖
                       █  ▐▛▚▖▐▌▐▌ ▐▌▐▌ ▐▌  █ ▐▌
                       █  ▐▌ ▝▜▌▐▛▀▘ ▐▌ ▐▌  █  ▝▀▚▖
                     ▗▄█▄▖▐▌  ▐▌▐▌   ▝▚▄▞▘  █ ▗▄▄▞▘

   Advanced inputs with builtin accessor/mutator callback system. Useful for
   systematizing the meshes/ objects attributes editions (transform/ light
   intensity) However more overkill and less suitable for more general usecase
   (hence the default classes above).

 */

typedef void (*input_callback_extra)(Scene *, Renderer *, void *);

// INT
template <typename T> class InputIntCallback : public Component {

public:
  InputIntCallback(T *target, Gui *gui, const char *label, int (*get)(T *),
                   void (*set)(T *, const int), input_callback_extra extra,
                   void *user_data)
      : Component(gui, label), target(target), get(get), set(set), extra(extra),
        user_data(user_data) {}
  bool draw() override;

private:
  T *target;
  int (*get)(T *);
  void (*set)(T *, const int);
  input_callback_extra extra;
  void *user_data;
  int value;
};

// FLOAT
template <typename T> class InputFloatCallback : public Component {

public:
  InputFloatCallback(T *target, Gui *gui, const char *label, float (*get)(T *),
                     void (*set)(T *, const float), input_callback_extra extra,
                     void *user_data)
      : Component(gui, label), target(target), get(get), set(set), extra(extra),
        user_data(user_data) {}
  bool draw() override;

private:
  T *target;
  float (*get)(T *);
  void (*set)(T *, const float);
  input_callback_extra extra;
  void *user_data;
  float value;
};

// VEC3
template <typename T> class InputVec3Callback : public Component {

public:
  InputVec3Callback(T *target, Gui *gui, const char *label,
                    void (*get)(T *, vec3), void (*set)(T *, const vec3),
                    input_callback_extra extra, void *user_data)
      : Component(gui, label), target(target), get(get), set(set), extra(extra),
        user_data(user_data) {}
  bool draw() override;

private:
  T *target;
  void (*get)(T *, vec3);
  void (*set)(T *, const vec3);
  input_callback_extra extra;
  void *user_data;
  vec3 value;
};

// VEC4
template <typename T> class InputVec4Callback : public Component {

public:
  InputVec4Callback(T *target, Gui *gui, const char *label,
                    void (*get)(T *, vec4), void (*set)(T *, const vec4),
                    input_callback_extra extra, void *user_data)
      : Component(gui, label), target(target), get(get), set(set), extra(extra),
        user_data(user_data) {}
  bool draw() override;

private:
  T *target;
  void (*get)(T *, vec4);
  void (*set)(T *, const vec4);
  input_callback_extra extra;
  void *user_data;
  vec4 value;
};

// COLOR
template <typename T> class InputColorCallback : public Component {

public:
  InputColorCallback(T *target, Gui *gui, const char *label,
                     void (*get)(T *, color), void (*set)(T *, const color),
                     input_callback_extra extra, void *user_data)
      : Component(gui, label), target(target), get(get), set(set), extra(extra),
        user_data(user_data) {}
  bool draw() override;

private:
  T *target;
  void (*get)(T *, color);
  void (*set)(T *, const color);
  input_callback_extra extra;
  void *user_data;
  color value;
};

template <typename T> class InputTextCallback : public Component {

public:
  InputTextCallback(T *target, Gui *gui, const char *label,
                    const int buffer_size, const InputFlag flag,
                    const char *(*get)(T *), void (*set)(T *, const char *),
                    input_callback_extra extra, void *user_data)
      : Component(gui, label), target(target), buffer_size(buffer_size),
        flag(flag), get(get), set(set), extra(extra), user_data(user_data) {}
  bool draw() override;

private:
  T *target;

  const char *(*get)(T *);
  void (*set)(T *, const char *);

  input_callback_extra extra;
  void *user_data;

  const int buffer_size;
  const InputFlag flag;
  static constexpr int max_buffer_size = 2048;
  char value[max_buffer_size];
};

template <typename T> bool InputIntCallback<T>::draw() {

  ImGui::Text("%s", label);

  if (get)
    value = get(target);

  name_t input_id;
  name_compose(input_id, "##%s_input", label);

  input_style_begin(gui);
  bool input = ImGui::DragInt(input_id, &value);
  input_style_end();

  if (input) {
    set(target, value);

    if (extra)
      extra(scene, renderer, user_data);
  }

  ImGui::Spacing();
  return input;
}

template <typename T> bool InputFloatCallback<T>::draw() {

  ImGui::Text("%s", label);

  if (get)
    value = get(target);

  name_t input_id;
  name_compose(input_id, "##%s_input", label);

  input_style_begin(gui);
  bool input = ImGui::DragFloat(input_id, &value, 0.1f);
  input_style_end();

  if (input) {

    if (set)
      set(target, value);

    if (extra)
      extra(scene, renderer, user_data);
  }

  ImGui::Spacing();

  return input;
}

template <typename T> bool InputVec3Callback<T>::draw() {

  ImGui::Text("%s", label);
  if (get)
    get(target, value);

  input_style_begin(gui);
  for (uint8_t i = 0; i < 3; i++) {
    name_t input_id;
    name_compose(input_id, "##%s%d", label, i);

    if (ImGui::DragFloat(input_id, &value[i], 0.1f)) {
      if (set)
        set(target, value);

      if (extra)
        extra(scene, renderer, user_data);
    }
  }
  input_style_end();

  ImGui::Spacing();

  return false;
}

template <typename T> bool InputVec4Callback<T>::draw() {

  ImGui::Text("%s", label);

  if (get)
    get(target, value);

  input_style_begin(gui);
  for (uint8_t i = 0; i < 4; i++) {
    name_t input_id;
    name_compose(input_id, "##%s%d", label, i);

    if (ImGui::DragFloat(input_id, &value[i], 0.1f)) {

      if (set)
        set(target, value);

      if (extra)
        extra(scene, renderer, user_data);
    }
  }
  input_style_end();

  ImGui::Spacing();

  return false;
}

template <typename T> bool InputColorCallback<T>::draw() {

  ImGui::Text("%s", label);

  if (get)
    get(target, value);

  name_t input_id;
  name_compose(input_id, "##%s_input", label);

  input_style_begin(gui);
  bool input = ImGui::ColorPicker4(
      input_id, value,
      ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview |
          ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHex);
  input_style_end();

  if (input) {

    if (set)
      set(target, value);

    if (extra)
      extra(scene, renderer, user_data);
  }

  ImGui::Spacing();

  return input;
}

template <typename T> bool InputTextCallback<T>::draw() {

  ImGui::Text("%s", label);

  const char *val;

  if (get)
    val = get(target);

  snprintf(value, max_buffer_size, "%s", val);

  name_t input_id;
  name_compose(input_id, "##%s_input", label);

  if (flag & InputFlag_SpanFullWidth)
    ImGui::SetNextItemWidth(-FLT_MIN);

  input_style_begin(gui);
  bool input =
      ImGui::InputText(input_id, value, glm_imin(buffer_size, max_buffer_size));
  input_style_end();

  if (input) {

    if (set)
      set(target, value);

    if (extra)
      extra(scene, renderer, user_data);
  }

  if (ImGui::IsItemActivated())
    g_input.locked = InputLockState_Keyboard;

  if (ImGui::IsItemDeactivated())
    g_input.locked = InputLockState_Unlocked;

  ImGui::Spacing();

  return input;
}

} // namespace UI

#endif
