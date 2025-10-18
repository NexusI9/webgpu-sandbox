#ifndef _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_WORLD_H_
#define _SCENE_EDTIOR_UI_WINDOW_INSPECTOR_WORLD_H_

#include "runtime/scene/editor/ui/windows/inspector/tab.hpp"

namespace UI {

class WorldTab : public InspectorTab {

public:
  WorldTab(Scene *scene, const SceneEditorUIIcon icon, const char *label)
      : InspectorTab(scene, icon, label) {}
  void draw() override;

private:
  static constexpr struct {
    TextureResolution resolution;
    const char *label;
  } fixed_resolutions[] = {
      {TextureResolution_Undefined, "Dynamic"},
      {TextureResolution_16, "16 x 16"},
      {TextureResolution_32, "32 x 32 "},
      {TextureResolution_64, "64 x 64"},
      {TextureResolution_128, "128 x 128"},
      {TextureResolution_256, "256 x 256"},
      {TextureResolution_512, "512 x 512"},
      {TextureResolution_1024, "1024 x 1024"},
      {TextureResolution_2048, "2048 x 2048"},
  };

};

} // namespace UI

#endif
