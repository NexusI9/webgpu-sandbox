#include "core.h"

Theme g_theme = {0};

ThemeStatus theme_init(Theme *theme, const ThemeDescriptor *desc) {
  
  theme->label = desc->label;
  theme->colors = desc->colors;
  theme->sizes = desc->sizes;
  theme->dpi = desc->dpi;

  return ThemeStatus_Success;
}
