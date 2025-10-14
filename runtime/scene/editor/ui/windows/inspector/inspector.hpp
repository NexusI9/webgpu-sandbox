#ifndef _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_H_
#define _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_H_

#include "../core.hpp"
#include "backend/context.h"
#include "backend/registry.h"
#include "backend/ssbo.h"
#include "imgui/imgui_impl_wgpu.h"
#include "runtime/scene/editor/ui/components/input.hpp"
#include "runtime/scene/editor/ui/core.h"
#include "runtime/scene/renderer/core.h"
#include "runtime/viewport/core.h"

namespace UI {

template <typename T> struct InspectorTreeIntAttribute {
  const char *label;
  void (*mutator_callback)(T *, int);
  int (*accessor_callback)(T *);
  void (*extra_callback)(Scene *, void *);
  void *user_data;
};

template <typename T> struct InspectorTreeFloatAttribute {
  const char *label;
  void (*mutator_callback)(T *, float);
  float (*accessor_callback)(T *);
  void (*extra_callback)(Scene *, void *);
  void *user_data;
};

template <typename T> struct InspectorTreeVec3Attribute {
  const char *label;
  void (*mutator_callback)(T *, vec3);
  void (*accessor_callback)(T *, vec3);
  void (*extra_callback)(Scene *, void *);
  void *user_data;
};

template <typename T> struct InspectorTreeVec4Attribute {
  const char *label;
  void (*mutator_callback)(T *, vec4);
  void (*accessor_callback)(T *, vec4);
  void (*extra_callback)(Scene *, void *);
  void *user_data;
};

#define INSPECTOR_TREE_CAPACITY 12

template <typename T> struct InspectorTreeList {

  const char *label;

  struct {
    size_t length;
    InspectorTreeIntAttribute<T> entries[INSPECTOR_TREE_CAPACITY];
  } int_list;

  struct {
    size_t length;
    InspectorTreeFloatAttribute<T> entries[INSPECTOR_TREE_CAPACITY];
  } float_list;

  struct {
    size_t length;
    InspectorTreeVec3Attribute<T> entries[INSPECTOR_TREE_CAPACITY];
  } vec3_list;

  struct {
    size_t length;
    InspectorTreeVec4Attribute<T> entries[INSPECTOR_TREE_CAPACITY];
  } vec4_list;

  struct {
    size_t length;
    InspectorTreeVec4Attribute<T> entries[INSPECTOR_TREE_CAPACITY];
  } color_list;
};

template <typename T>
static inline void
inspector_tree_list_draw(T *target, const InspectorTreeList<T> *list,
                         Scene *scene, SSBOType ssbo_type, ssbo_id_t ssbo_id) {

  ImGuiTreeNodeFlags flags =
      ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen;

  ImGui::PushStyleVar(
      ImGuiStyleVar_FramePadding,
      ImVec2(scene->editor.ui.size[SceneEditorUISize_Tree_PaddingH],
             scene->editor.ui.size[SceneEditorUISize_Tree_PaddingV]));

  if (ImGui::TreeNodeEx(list->label, flags)) {
    size_t i;

    for (i = 0; i < list->int_list.length; i++)
      UI::InputInt<T>(target, scene, list->int_list.entries[i].label,
                      list->int_list.entries[i].accessor_callback,
                      list->int_list.entries[i].mutator_callback,
                      list->int_list.entries[i].extra_callback,
                      list->int_list.entries[i].user_data, ssbo_type, ssbo_id)
          .draw();

    for (i = 0; i < list->float_list.length; i++)
      UI::InputFloat<T>(target, scene, list->float_list.entries[i].label,
                        list->float_list.entries[i].accessor_callback,
                        list->float_list.entries[i].mutator_callback,
                        list->float_list.entries[i].extra_callback,
                        list->float_list.entries[i].user_data, ssbo_type,
                        ssbo_id)
          .draw();

    for (i = 0; i < list->vec3_list.length; i++)
      UI::InputVec3<T>(target, scene, list->vec3_list.entries[i].label,
                       list->vec3_list.entries[i].accessor_callback,
                       list->vec3_list.entries[i].mutator_callback,
                       list->vec3_list.entries[i].extra_callback,
                       list->vec3_list.entries[i].user_data, ssbo_type, ssbo_id)
          .draw();

    for (i = 0; i < list->vec4_list.length; i++)
      UI::InputVec4<T>(target, scene, list->vec4_list.entries[i].label,
                       list->vec4_list.entries[i].accessor_callback,
                       list->vec4_list.entries[i].mutator_callback,
                       list->vec4_list.entries[i].extra_callback,
                       list->vec4_list.entries[i].user_data, ssbo_type, ssbo_id)
          .draw();

    for (i = 0; i < list->color_list.length; i++)
      UI::InputColor<T>(target, scene, list->color_list.entries[i].label,
                        list->color_list.entries[i].accessor_callback,
                        list->color_list.entries[i].mutator_callback,
                        list->color_list.entries[i].extra_callback,
                        list->color_list.entries[i].user_data, ssbo_type,
                        ssbo_id)
          .draw();

    ImGui::TreePop();
  }
  ImGui::PopStyleVar();
}

class InspectorTab : public Window {

public:
  InspectorTab(Scene *scene, const SceneEditorUIIcon icon, const char *label)
      : Window(scene, label), icon(icon), tooltip(label) {}
  virtual void draw() = 0;
  const SceneEditorUIIcon icon;
  const char *tooltip;
};

typedef struct {
  const ProfilerLatencyType type;
  const char *label;
  const float *color;
  double value;
} ClockTabBar;

class ClockTab : public InspectorTab {

public:
  ClockTab(Scene *scene, const SceneEditorUIIcon icon, const char *label)
      : InspectorTab(scene, icon, label) {}
  void draw() override;

private:
  static constexpr double max_value = 10.0f;
  static constexpr color color = {0.3f, 0.3f, 0.5f, 1.0f};
};

class SceneTab : public InspectorTab {

public:
  SceneTab(Scene *scene, const SceneEditorUIIcon icon, const char *label)
      : InspectorTab(scene, icon, label) {}
  void draw() override;

private:
  int width = context_width();
  int height = context_height();
  float fov = viewport_fov(&scene->viewport);
  float near_clip = viewport_near_clip(&scene->viewport);
  float far_clip = viewport_far_clip(&scene->viewport);
  double dpi = scene_renderer_dpi(&scene->renderer);
  RenderPipelineMultisampleCount multisample = context_multisample();
};

class InfoTab : public InspectorTab {

public:
  InfoTab(Scene *scene, const SceneEditorUIIcon icon, const char *label)
      : InspectorTab(scene, icon, label) {}
  void draw() override;

private:
  static constexpr const char *backend_label[] = {
      "Undefined", "Null",   "WebGPU", "D3D11",    "D3D12",
      "Metal",     "Vulkan", "OpenGL", "OpenGLES", "Force32",
  };

  static constexpr const char *adapter_type_label[] = {
      "Discrete GPU", "Integrated GPU", "CPU", "Unknown", "Force32",
  };
};

class ObjectTab : public InspectorTab {

public:
  ObjectTab(Scene *scene, const SceneEditorUIIcon icon, const char *label)
      : InspectorTab(scene, icon, label) {}
  void draw() override;

private:
  static constexpr uint8_t valid_type_len = 8;
  static constexpr RegEntryType valid_type[valid_type_len] = {
      RegEntryType_Mesh,
      RegEntryType_PointLight,
      RegEntryType_AmbientLight,
      RegEntryType_SunLight,
      RegEntryType_SpotLight,
      RegEntryType_Camera,
      RegEntryType_ProbeReflectionPlane,
      RegEntryType_ProbeReflectionGrid,
  };

  inline bool is_valid_type(const RegEntryType);
  inline reg_id_t set_active_target();

private:
};

class Inspector : public Window {

public:
  Inspector(Scene *scene, const char *label)
      : Window(scene, label),
        object_tab(scene, SceneEditorUIIcon_Properties_Object, "Object"),
        info_tab(scene, SceneEditorUIIcon_Properties_Chip,
                 "Device Information"),
        scene_tab(scene, SceneEditorUIIcon_Properties_Scene, "Scene"),
        clock_tab(scene, SceneEditorUIIcon_Properties_Clock, "Latencies") {

    tabs[0] = &scene_tab;
    tabs[1] = &info_tab;
    tabs[2] = &object_tab;
    tabs[3] = &clock_tab;
  }

  void draw();

private:
  static constexpr uint8_t tab_count = 4;
  UI::InspectorTab *tabs[tab_count];
  UI::SceneTab scene_tab;
  UI::InfoTab info_tab;
  UI::ObjectTab object_tab;
  UI::ClockTab clock_tab;
};

} // namespace UI

#endif
