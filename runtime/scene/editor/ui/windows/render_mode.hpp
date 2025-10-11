#ifndef _SCENE_EDTIOR_UI_WINDOW_RENDER_MODE_H_
#define _SCENE_EDTIOR_UI_WINDOW_RENDER_MODE_H_

#include "core.hpp"
#include "runtime/scene/draw.h"
#include "runtime/scene/editor/ui/components/button_group.hpp"
#include "runtime/scene/renderer/core.h"

namespace UI {

class RenderMode : public Window {

  static constexpr SceneRendererDrawMode
      draw_modes[SCENE_RENDERER_DRAW_MODE_COUNT] = {
          SceneRendererDrawMode_Boundbox,
          SceneRendererDrawMode_Wireframe,
          SceneRendererDrawMode_Solid,
          SceneRendererDrawMode_Texture,
      };

  static void update_render_mode(Scene *scene, void *mode) {
    scene_set_draw_mode(scene, *(SceneRendererDrawMode *)mode);
  }

public:
  RenderMode(Scene *scene, const char *label) : Window(scene, label) {}
  void draw() override;

  static constexpr int count = SCENE_RENDERER_DRAW_MODE_COUNT;
  static constexpr ButtonGroupItem buttons[count] = {
      {
          "mode_boundbox",
          SceneEditorUIIcon_RenderMode_Boundbox,
          update_render_mode,
          (void *)&draw_modes[0],
          "Boundbox rendering",
      },
      {
          "mode_wireframe",
          SceneEditorUIIcon_RenderMode_Wireframe,
          update_render_mode,
          (void *)&draw_modes[1],
          "Wireframe rendering",
      },
      {
          "mode_solid",
          SceneEditorUIIcon_RenderMode_Solid,
          update_render_mode,
          (void *)&draw_modes[2],
          "Solid rendering",
      },
      {
          "mode_texture",
          SceneEditorUIIcon_RenderMode_Texture,
          update_render_mode,
          (void *)&draw_modes[3],
          "Texture rendering",
      },
  };
};

} // namespace UI

#endif
