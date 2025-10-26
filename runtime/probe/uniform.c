#include "runtime/probe/uniform.h"
#include "backend/ubo.h"
#include "core.h"

ProbeListSlot probe_list_uniform_new_entry(ProbeListUniform *list,
                                           const ProbeType type) {

  switch (type) {

  case ProbeType_ReflectionPlane:

    if (list->reflection_plane_count < PROBE_LIST_ENTRIES_CAPACITY)
      return (ProbeListSlot){
          .uniform =
              {.reflection_plane =
                   &list->reflection_plane[list->reflection_plane_count]},
          .offset = probe_list_uniform_offset(list->reflection_plane_count,
                                              ProbeType_ReflectionPlane),
          .id = list->reflection_plane_count++,
      };
    break;

  case ProbeType_ReflectionProbe:
    if (list->reflection_probe_count < PROBE_LIST_ENTRIES_CAPACITY)
      return (ProbeListSlot){
          .uniform =
              {.reflection_probe =
                   &list->reflection_probe[list->reflection_probe_count]},
          .offset = probe_list_uniform_offset(list->reflection_probe_count,
                                              ProbeType_ReflectionProbe),
          .id = list->reflection_probe_count++,
      };
    break;
  }
  logger_add(LoggerFlag_Error, "Couldn't create new probe uniform slot.");

  return (ProbeListSlot){.uniform = 0, .id = UBO_INDEX_UNFOUND, .offset = 0};
}
