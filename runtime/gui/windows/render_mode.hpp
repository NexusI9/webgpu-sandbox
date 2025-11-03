#ifndef _GUI_WINDOW_RENDER_MODE_H_
#define _GUI_WINDOW_RENDER_MODE_H_

#include "backend/theme/core.h"
#include "core.hpp"
#include "runtime/scene/draw.h"
#include "runtime/gui/components/button_group.hpp"
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
  RenderMode(Gui* gui, const char *label) : Window(gui, label) {}
  void draw() override;

  static constexpr int count = SCENE_RENDERER_DRAW_MODE_COUNT;
  static constexpr ButtonGroupItem buttons[count] = {
      {
          "mode_boundbox",
          ThemeIcon_RenderMode_Boundbox,
          update_render_mode,
          (void *)&draw_modes[0],
          "Boundbox rendering",
      },
      {
          "mode_wireframe",
          ThemeIcon_RenderMode_Wireframe,
          update_render_mode,
          (void *)&draw_modes[1],
          "Wireframe rendering",
      },
      {
          "mode_solid",
          ThemeIcon_RenderMode_Solid,
          update_render_mode,
          (void *)&draw_modes[2],
          "Solid rendering",
      },
      {
          "mode_texture",
          ThemeIcon_RenderMode_Texture,
          update_render_mode,
          (void *)&draw_modes[3],
          "Texture rendering",
      },
  };
};

} // namespace UI

#endif
