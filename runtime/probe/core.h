#ifndef _PROBE_CORE_H_
#define _PROBE_CORE_H_

#include "runtime/probe/reflection/grid.h"
#include "runtime/probe/reflection/plane.h"


typedef struct {
  ProbeReflectionGridList reflection_probe;
  ProbeReflectionPlaneList reflection_plane;
  UBOSlot ubo_slot; // ubo slot of the ProbeListUniform
} ProbeList;

void probe_list_update_uniform(ProbeList *);

#endif
