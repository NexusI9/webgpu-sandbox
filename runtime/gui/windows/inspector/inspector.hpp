#ifndef _GUI_WINDOW_INSPECTOR_H_
#define _GUI_WINDOW_INSPECTOR_H_

#include "../core.hpp"
#include "backend/context.h"
#include "backend/registry.h"
#include "backend/ubo.h"
#include "imgui/imgui_impl_wgpu.h"
#include "runtime/gui/components/input.hpp"
#include "runtime/gui/core.h"
#include "runtime/gui/windows/inspector/inspector.clock.hpp"
#include "runtime/gui/windows/inspector/inspector.information.hpp"
#include "runtime/gui/windows/inspector/inspector.object.hpp"
#include "runtime/gui/windows/inspector/inspector.render.hpp"
#include "runtime/gui/windows/inspector/inspector.world.hpp"
#include "runtime/gui/windows/inspector/tab.hpp"
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
                                            Gui *gui) {

  ImGuiTreeNodeFlags flags =
      ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_DefaultOpen;

  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,
                      ImVec2(theme_size(gui->theme, ThemeSize_Tree_PaddingH),
                             theme_size(gui->theme, ThemeSize_Tree_PaddingV)));

  Scene *scene = gui->active_scene;

  if (ImGui::TreeNodeEx(list->label, flags)) {
    size_t i;

    ImGui::PushItemWidth(-1);
    for (i = 0; i < list->int_list.length; i++)
      UI::InputIntCallback<T>(target, gui, list->int_list.entries[i].label,
                              list->int_list.entries[i].accessor_callback,
                              list->int_list.entries[i].mutator_callback,
                              list->int_list.entries[i].extra_callback,
                              list->int_list.entries[i].user_data)
          .draw();

    for (i = 0; i < list->float_list.length; i++)
      UI::InputFloatCallback<T>(target, gui, list->float_list.entries[i].label,
                                list->float_list.entries[i].accessor_callback,
                                list->float_list.entries[i].mutator_callback,
                                list->float_list.entries[i].extra_callback,
                                list->float_list.entries[i].user_data)
          .draw();

    for (i = 0; i < list->vec3_list.length; i++)
      UI::InputVec3Callback<T>(target, gui, list->vec3_list.entries[i].label,
                               list->vec3_list.entries[i].accessor_callback,
                               list->vec3_list.entries[i].mutator_callback,
                               list->vec3_list.entries[i].extra_callback,
                               list->vec3_list.entries[i].user_data)
          .draw();

    for (i = 0; i < list->vec4_list.length; i++)
      UI::InputVec4Callback<T>(target, gui, list->vec4_list.entries[i].label,
                               list->vec4_list.entries[i].accessor_callback,
                               list->vec4_list.entries[i].mutator_callback,
                               list->vec4_list.entries[i].extra_callback,
                               list->vec4_list.entries[i].user_data)
          .draw();

    for (i = 0; i < list->color_list.length; i++)
      UI::InputColorCallback<T>(target, gui, list->color_list.entries[i].label,
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
  Inspector(Gui *ui, const char *label)
      : Window(ui, label),
        object_tab(gui, ThemeIcon_Properties_Object, "Object"),
        info_tab(gui, ThemeIcon_Properties_Chip, "Information"),
        render_tab(gui, ThemeIcon_Properties_Scene, "Render"),
        clock_tab(gui, ThemeIcon_Properties_Clock, "Latencies"),
        world_tab(gui, ThemeIcon_Properties_Earth, "World") {

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
