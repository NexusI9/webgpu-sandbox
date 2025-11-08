#ifndef _GUI_COMPONENT_COMBOBOX_RESOLUTION_H_
#define _GUI_COMPONENT_COMBOBOX_RESOLUTION_H_

#include "runtime/gui/components/core.hpp"
#include "runtime/gui/components/input.hpp"
#include "runtime/texture/core.h"
namespace UI {

typedef void (*combobox_resolution_on_select)(
    Scene *, Renderer *, const TextureResolution reference_resolution);

class ComboboxResolution : public Component {

public:
  ComboboxResolution(Gui *gui, const char *label, const char *default_value,
                     const InputStyle *style,
                     const TextureResolution current_resolution,
                     combobox_resolution_on_select on_select_callback)
      : Component(gui, label), current_resolution(current_resolution),
        default_value(default_value), on_select_callback(on_select_callback),
        style(style) {}

  bool draw() override;

private:
  const InputStyle *style;
  const char *default_value;
  const TextureResolution current_resolution;
  combobox_resolution_on_select on_select_callback;

  static constexpr uint8_t fixed_resolution_count = 8;
  static constexpr struct {
    TextureResolution resolution;
    const char *label;
  } fixed_resolutions[fixed_resolution_count] = {
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
