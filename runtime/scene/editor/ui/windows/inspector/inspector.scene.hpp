#ifndef _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_SCENE_H_
#define _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_SCENE_H_

#include "runtime/scene/editor/ui/windows/inspector/tab.hpp"

namespace UI {

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
  float dpi = (float)scene_renderer_dpi(&scene->renderer);
  RenderPipelineMultisampleCount multisample = context_multisample();

  static constexpr RenderPipelineMultisampleCount multisample_count[2] = {
      PipelineMultisampleCount_1x,
      PipelineMultisampleCount_4x,
  };
};

} // namespace UI

#endif
