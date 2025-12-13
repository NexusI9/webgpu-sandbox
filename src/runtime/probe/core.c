#include "core.h"


void probe_list_update_uniform(ProbeList *list) {
  
  ProbeListUniform *uniform = (ProbeListUniform *)list->ubo_slot.uniform;
  
  uniform->reflection_probe_count = list->reflection_probe.count;
  uniform->reflection_plane_count = list->reflection_plane.count;
}
