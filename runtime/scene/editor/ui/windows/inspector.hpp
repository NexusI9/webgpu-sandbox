#ifndef _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_H_
#define _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_H_

#include "backend/context.h"
#include "backend/registry.h"
#include "core.hpp"
#include "runtime/scene/editor/ui/core.h"
#include "runtime/scene/renderer/core.h"
#include "runtime/viewport/core.h"

namespace UI {

class InspectorTab : public Window {

public:
  InspectorTab(Scene *scene, const SceneEditorUIIcon icon, const char *label)
      : Window(scene, label), icon(icon), tooltip(label) {}
  virtual void draw() = 0;
  const SceneEditorUIIcon icon;
  const char *tooltip;
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

class SettingTab : public InspectorTab {

public:
  SettingTab(Scene *scene, const SceneEditorUIIcon icon, const char *label)
      : InspectorTab(scene, icon, label) {}
  void draw() override;

private:
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
        setting_tab(scene, SceneEditorUIIcon_Properties_Setting, "Setting"),
        scene_tab(scene, SceneEditorUIIcon_Properties_Scene, "Scene") {

    tabs[0] = &scene_tab;
    tabs[1] = &setting_tab;
    tabs[2] = &object_tab;
  }

  void draw();

private:
  static constexpr uint8_t tab_count = 3;
  UI::InspectorTab *tabs[tab_count];
  UI::SceneTab scene_tab;
  UI::SettingTab setting_tab;
  UI::ObjectTab object_tab;
};

} // namespace UI

#endif
