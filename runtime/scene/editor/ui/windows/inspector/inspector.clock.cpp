#include "inspector.clock.hpp"
#include "runtime/scene/editor/ui/components/time_bar.hpp"
#include "runtime/scene/editor/ui/theme/theme.default.h"

int qsort_callback(const void *a, const void *b) {
  const UI::ClockTabBar *A = (const UI::ClockTabBar *)a;
  const UI::ClockTabBar *B = (const UI::ClockTabBar *)b;

  if (B->value > A->value)
    return 1;
  if (B->value < A->value)
    return -1;
  return 0;
}

void UI::ClockTab::draw() {

  ClockTabBar bars[] = {
      {
          ProfilerLatencyType_ShadowPass,
          "Shadow Pass",
          color,
      },
      {
          ProfilerLatencyType_ReflectionPass,
          "Reflection Pass",
          color,
      },
      {
          ProfilerLatencyType_BlitPass,
          "Blit Pass",
          color,
      },
      {
          ProfilerLatencyType_KawasePass,
          "Kawase Pass",
          color,
      },
      {
          ProfilerLatencyType_BloomPass,
          "Bloom Pass",
          color,
      },
  };

  // fetch all value
  const size_t length = sizeof(bars) / sizeof(ClockTabBar);
  for (int i = 0; i < length; i++)
    bars[i].value =
        profiler_latency_get_elapsed(&scene->renderer.profiler, bars[i].type);

  // sort
  qsort(bars, length, sizeof(ClockTabBar), qsort_callback);

  TimeBarStyle style = {
      .background =
          (ImVec4 &)theme_default_color[THEME_DEFAULT_COLOR_SURFACE_LOW],
      .bar = (ImVec4 &)color,
      .border_radius = 10.0f,
      .height = 40.0f,
  };

  // display time bar
  for (int i = 0; i < length; i++) {
    UI::TimeBar(scene, bars[i].label, bars[i].value, max_value, &style).draw();
    ImGui::Spacing();
  }
}
