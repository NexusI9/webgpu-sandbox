#ifndef _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_H_
#define _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_H_

#include "../core.hpp"
#include "backend/context.h"
#include "backend/registry.h"
#include "backend/ubo.h"
#include "imgui/imgui_impl_wgpu.h"
#include "runtime/scene/editor/ui/components/input.hpp"
#include "runtime/scene/editor/ui/core.h"
#include "runtime/scene/editor/ui/windows/inspector/inspector.clock.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.information.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.object.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.render.hpp"
#include "runtime/scene/editor/ui/windows/inspector/inspector.world.hpp"
#include "runtime/scene/editor/ui/windows/inspector/tab.hpp"
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
static inline void inspector_tree_list_draw(T *target,
                                            const InspectorTreeList<T> *list,
                                            Scene *scene) {

  ImGuiTreeNodeFlags flags =
      ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen;

  ImGui::PushStyleVar(
      ImGuiStyleVar_FramePadding,
      ImVec2(scene->editor.ui.size[SceneEditorUISize_Tree_PaddingH],
             scene->editor.ui.size[SceneEditorUISize_Tree_PaddingV]));

  if (ImGui::TreeNodeEx(list->label, flags)) {
    size_t i;

    ImGui::PushItemWidth(-1);
    for (i = 0; i < list->int_list.length; i++)
      UI::InputIntCallback<T>(target, scene, list->int_list.entries[i].label,
                              list->int_list.entries[i].accessor_callback,
                              list->int_list.entries[i].mutator_callback,
                              list->int_list.entries[i].extra_callback,
                              list->int_list.entries[i].user_data)
          .draw();

    for (i = 0; i < list->float_list.length; i++)
      UI::InputFloatCallback<T>(target, scene,
                                list->float_list.entries[i].label,
                                list->float_list.entries[i].accessor_callback,
                                list->float_list.entries[i].mutator_callback,
                                list->float_list.entries[i].extra_callback,
                                list->float_list.entries[i].user_data)
          .draw();

    for (i = 0; i < list->vec3_list.length; i++)
      UI::InputVec3Callback<T>(target, scene, list->vec3_list.entries[i].label,
                               list->vec3_list.entries[i].accessor_callback,
                               list->vec3_list.entries[i].mutator_callback,
                               list->vec3_list.entries[i].extra_callback,
                               list->vec3_list.entries[i].user_data)
          .draw();

    for (i = 0; i < list->vec4_list.length; i++)
      UI::InputVec4Callback<T>(target, scene, list->vec4_list.entries[i].label,
                               list->vec4_list.entries[i].accessor_callback,
                               list->vec4_list.entries[i].mutator_callback,
                               list->vec4_list.entries[i].extra_callback,
                               list->vec4_list.entries[i].user_data)
          .draw();

    for (i = 0; i < list->color_list.length; i++)
      UI::InputColorCallback<T>(target, scene,
                                list->color_list.entries[i].label,
                                list->color_list.entries[i].accessor_callback,
                                list->color_list.entries[i].mutator_callback,
                                list->color_list.entries[i].extra_callback,
                                list->color_list.entries[i].user_data)
          .draw();

    ImGui::PopItemWidth();
    ImGui::TreePop();
  }
  ImGui::PopStyleVar();
}

class Inspector : public Window {

public:
  Inspector(Scene *scene, const char *label)
      : Window(scene, label),
        object_tab(scene, SceneEditorUIIcon_Properties_Object, "Object"),
        info_tab(scene, SceneEditorUIIcon_Properties_Chip, "Information"),
        render_tab(scene, SceneEditorUIIcon_Properties_Scene, "Render"),
        clock_tab(scene, SceneEditorUIIcon_Properties_Clock, "Latencies"),
        world_tab(scene, SceneEditorUIIcon_Properties_Earth, "World") {

    tabs[0] = &render_tab;
    tabs[1] = &world_tab;
    tabs[2] = &object_tab;
    tabs[3] = &clock_tab;
    tabs[4] = &info_tab;

  }

  void draw();

private:
  static int active_tab;
  static constexpr uint8_t tab_count = 5;
  UI::InspectorTab *tabs[tab_count];
  UI::RenderTab render_tab;
  UI::InfoTab info_tab;
  UI::ObjectTab object_tab;
  UI::ClockTab clock_tab;
  UI::WorldTab world_tab;
};

} // namespace UI

#endif
