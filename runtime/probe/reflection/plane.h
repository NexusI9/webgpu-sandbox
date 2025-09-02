#ifndef _PROBE_REFLECTION_PLANE_H_
#define _PROBE_REFLECTION_PLANE_H_

#include "../backend/renderer/scene/render_pass/render_pass.h"
#include "./core.h"
#include "cglm/cglm.h"

#define PROBE_REFLECTION_PLANE_LIST_LAYER_COUNT 32

typedef struct {
  vec3 position;
  vec3 direction;
  vec3 scale;
  float near;
  float far;
  SSBOSlot ssbo_slot[PROBE_REFLECTION_SSBO_SLOT_COUNT];
  Projection views;
} ProbeReflectionPlane;

typedef struct {
  vec3 position;
  float _pad;
  vec3 direction;
  float _pad2;
  vec3 scale;
  float _pad3[53];
} __attribute__((aligned(16))) ProbeReflectionPlaneUniform;

typedef struct {
  ProbeReflectionPlane *entries;
  size_t capacity;
  size_t length;
  RenderPass pass;
} ProbeReflectionPlaneList;

DynamicListStatus
probe_reflection_plane_list_create(ProbeReflectionPlaneList *,
                                   const ProbeReflectionListDescriptor *);

DynamicListStatus probe_reflection_plane_list_insert(ProbeReflectionPlaneList *,
                                                     ProbeReflectionPlane *);

ProbeReflectionPlane *
probe_reflection_plane_list_new_entry(ProbeReflectionPlaneList *);

DynamicListStatus probe_reflection_plane_list_remove(ProbeReflectionPlaneList *,
                                                     ProbeReflectionPlane *);

DynamicListStatus
probe_reflection_plane_list_destroy(ProbeReflectionPlaneList *);

void probe_reflection_plane_list_draw(ProbeReflectionPlaneList *,
                                      ProbeReflectionListDebug *);

#endif
