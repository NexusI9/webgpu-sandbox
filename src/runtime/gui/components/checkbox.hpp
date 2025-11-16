#ifndef _GUI_CHECKBOX_H_
#define _GUI_CHECKBOX_H_

#include "runtime/gui/components/core.hpp"
#include "runtime/gui/core.h"

namespace UI {

class Checkbox : public Component {

  typedef void (*checkbox_on_change)(Scene *, Renderer *, bool, void *);

public:
  Checkbox(Gui *gui, const char *label, bool active, const ThemeIcon icon,
           checkbox_on_change on_change, void *user_data)
      : Component(gui, label), icon(icon), on_change(on_change),
        user_data(user_data), active(active) {}

  bool draw() override;

private:
  const ThemeIcon icon;
  checkbox_on_change on_change;
  void *user_data;
  bool active;
};

} // namespace UI

#endif
