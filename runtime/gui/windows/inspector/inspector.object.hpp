#ifndef _GUI_WINDOW_INSPECTOR_OBJECT_H_
#define _GUI_WINDOW_INSPECTOR_OBJECT_H_

#include "runtime/gui/windows/inspector/tab.hpp"

namespace UI {

class ObjectTab : public InspectorTab {

public:
  ObjectTab(Gui* gui, const ThemeIcon icon, const char *label)
      : InspectorTab(gui, icon, label) {}
  void draw() override;
  static RegEntry const *active_object;

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

} // namespace UI

#endif
