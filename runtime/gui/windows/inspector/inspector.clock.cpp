#include "inspector.clock.hpp"
#include "backend/profiler.h"
#include "backend/theme/core.h"
#include "runtime/gui/components/time_bar.hpp"

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
      {
          ProfilerLatencyType_CompositePass,
          "Composite Pass",
          color,
      },
  };

  // fetch all value
  const size_t length = 6;
  for (int i = 0; i < length; i++)
    bars[i].value =
        profiler_latency_get_elapsed(&scene->renderer.profiler, bars[i].type);

  // sort
  qsort(bars, length, sizeof(ClockTabBar), qsort_callback);

  TimeBarStyle style = {
      .background = (ImVec4 &)*theme_color(gui->theme, ThemeColor_Surface_Low),
      .bar = (ImVec4 &)color,
      .border_radius = 10.0f,
      .height = 40.0f,
  };

  // display time bar
  for (int i = 0; i < length; i++) {
    UI::TimeBar(gui, bars[i].label, bars[i].value, max_value, &style).draw();
    ImGui::Spacing();
  }
}
