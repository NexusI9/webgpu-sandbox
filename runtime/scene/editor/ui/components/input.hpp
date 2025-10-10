#ifndef _UI_INPUT_H_
#define _UI_INPUT_H_

#include "imgui/imgui.h"
#include "runtime/scene/editor/ui/windows/core.hpp"
#include "utils/color.h"
#include "utils/name.h"

namespace UI {

// INT
template <typename T> class InputInt : public Window {

public:
  InputInt(T *target, Scene *scene, const char *label, int (*get)(T *),
           void (*set)(T *, int))
      : Window(scene, label), target(target), label(label), get(get), set(set) {
  }
  void draw() override;

private:
  T *target;
  int (*get)(T *);
  void (*set)(T *, int);
  int value;
  const char *label;
};

// FLOAT
template <typename T> class InputFloat : public Window {

public:
  InputFloat(T *target, Scene *scene, const char *label, float (*get)(T *),
             void (*set)(T *, float))
      : Window(scene, label), target(target), label(label), get(get), set(set) {
  }
  void draw() override;

private:
  T *target;
  float (*get)(T *);
  void (*set)(T *, float);
  float value;
  const char *label;
};

// VEC3
template <typename T> class InputVec3 : public Window {

public:
  InputVec3(T *target, Scene *scene, const char *label, void (*get)(T *, vec3),
            void (*set)(T *, vec3))
      : Window(scene, label), target(target), label(label), get(get), set(set) {
  }
  void draw() override;

private:
  T *target;
  void (*get)(T *, vec3);
  void (*set)(T *, vec3);
  vec3 value;
  const char *label;
};

// COLOR
template <typename T> class InputVec4 : public Window {

public:
  InputVec4(T *target, Scene *scene, const char *label, void (*get)(T *, vec4),
            void (*set)(T *, vec4))
      : Window(scene, label), target(target), label(label), get(get), set(set) {
  }
  void draw() override;

private:
  T *target;
  void (*get)(T *, vec4);
  void (*set)(T *, vec4);
  vec4 value;
  const char *label;
};

// COLOR
template <typename T> class InputColor : public Window {

public:
  InputColor(T *target, Scene *scene, const char *label,
             void (*get)(T *, color), void (*set)(T *, color))
      : Window(scene, label), target(target), label(label), get(get), set(set) {
  }
  void draw() override;

private:
  T *target;
  void (*get)(T *, color);
  void (*set)(T *, color);
  color value;
  const char *label;
};

template <typename T> void InputInt<T>::draw() {

  ImGui::Text("%s", label);
  value = get(target);

  name_t input_id;
  name_compose(input_id, "##%s_input", label);

  if (ImGui::DragInt(input_id, &value))
    set(target, value);

  ImGui::Spacing();
}

template <typename T> void InputFloat<T>::draw() {

  ImGui::Text("%s", label);
  value = get(target);

  name_t input_id;
  name_compose(input_id, "##%s_input", label);

  if (ImGui::DragFloat(input_id, &value))
    set(target, value);

  ImGui::Spacing();
}

template <typename T> void InputVec3<T>::draw() {

  ImGui::Text("%s", label);
  get(target, value);

  for (uint8_t i = 0; i < 3; i++) {
    name_t input_id;
    name_compose(input_id, "##%s%d", label, i);

    if (ImGui::DragFloat(input_id, &value[i]))
      set(target, value);
  }

  ImGui::Spacing();
}

template <typename T> void InputVec4<T>::draw() {

  ImGui::Text("%s", label);
  get(target, value);

  for (uint8_t i = 0; i < 4; i++) {
    name_t input_id;
    name_compose(input_id, "##%s%d", label, i);

    if (ImGui::DragFloat(input_id, &value[i]))
      set(target, value);
  }

  ImGui::Spacing();
}

template <typename T> void InputColor<T>::draw() {

  ImGui::Text("%s", label);
  get(target, value);

  name_t input_id;
  name_compose(input_id, "##%s_input", label);

  if (ImGui::ColorPicker4(input_id, value))
    set(target, value);

  ImGui::Spacing();
}

} // namespace UI

#endif
