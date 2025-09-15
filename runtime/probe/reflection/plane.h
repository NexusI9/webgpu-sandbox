#ifndef _PROBE_REFLECTION_PLANE_H_
#define _PROBE_REFLECTION_PLANE_H_

#include <cglm/types.h>
#include <stddef.h>
#include <stdint.h>

#include "../runtime/scene/renderer/render_pass/render_pass.h"
#include "./core.h"
#include "cglm/cglm.h"
#include "../backend/ssbo.h"
#include "../runtime/camera/core.h"
#include "../runtime/geometry/aabb/aabb.h"
#include "../runtime/mesh/core.h"
#include "../runtime/scene/renderer/render_pass/core.h"
#include "../utils/dyli.h"

#define PROBE_REFLECTION_PLANE_LIST_LAYER_COUNT 32

typedef struct {
  vec3 position;
  vec3 normal;
  float signed_distance;
  vec3 scale;
  vec3 tangent;
  vec3 bitangent;
  float near;
  float far;
  float distance;
  SSBOSlot ssbo_slot[PROBE_REFLECTION_SSBO_SLOT_COUNT];
  Camera camera;
  Camera const *ref_camera;
  AABB boundbox;
  uint32_t texture_layer;
  MeshRefList excluded_meshes;
} ProbeReflectionPlane;

typedef struct {
  vec3 position;
  float near;
  vec3 normal;
  float far;
  vec3 scale;
  float distance;
  vec3 tangent;
  float signed_distance;
  vec3 bitangent;
  uint32_t texture_layer;
  mat4 view;
  float _pad1[28];
} __attribute__((aligned(16))) ProbeReflectionPlaneUniform;

typedef struct {
  ProbeReflectionPlane *entries;
  size_t capacity;
  size_t length;
  RenderPass pass;
} ProbeReflectionPlaneList;

typedef struct {
  float near;
  float far;
  vec3 scale;
  vec3 normal;
  vec3 position;
  float distance;
  Camera const *camera;
} ProbeReflectionPlaneDescriptor;

/* === Plane List === */
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

void probe_reflection_plane_list_draw_callback(void *);

/* === Plane === */
void probe_reflection_plane_create(ProbeReflectionPlane *,
                                   ProbeReflectionPlaneDescriptor *);

void probe_reflection_plane_disable_mesh(ProbeReflectionPlane *,
                                              Mesh *);
void probe_reflection_plane_enable_mesh(ProbeReflectionPlane *,
                                             Mesh *);

void probe_reflection_plane_update_uniform(ProbeReflectionPlane *);
void probe_reflection_plane_update_camera(ProbeReflectionPlane *);
void probe_reflection_plane_update_boundbox(ProbeReflectionPlane *);

#endif
