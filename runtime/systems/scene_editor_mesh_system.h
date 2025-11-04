#ifndef _SCENE_EDITOR_MESH_H_
#define _SCENE_EDITOR_MESH_H_

#include "backend/renderer/core.h"
#include "backend/ubo.h"
#include "runtime/light/list.h"
#include "runtime/probe/core.h"
#include "runtime/scene/core.h"
#include "runtime/scene/editor_mesh/core.h"

void sem_list_system_toggle_visibility(SceneEditorMeshListArray *, Renderer *,
                                       const RegEntryType *, size_t, bool);

// clang-format off
#define SEM_GENERIC_ITEMS(_)                   \
  _(  Camera,         camera                 )

#define SEM_LIGHT_ITEMS(_)                     \
  _(  PointLight,     point_light            ) \
  _(  AmbientLight,   ambient_light          ) \
  _(  SunLight,       sun_light              ) \
  _(  SpotLight,      spot_light             ) 

/* Shadow (update shadow pass on transform) */
#define SEM_SHADOW_ITEMS(_)\
  _(  PointLight,     point_light_shadow     ) \
  _(  SunLight,       sun_light_shadow       ) \
  _(  SpotLight,      spot_light_shadow      )

// clang-format on

// Mutator (general usage)
#define SEM_LIST_GENERIC_MUTATOR(Type, Label)                                  \
  void sem_list_system_##Label##_set_position(const SEMListTransform *);       \
  void sem_list_system_##Label##_set_rotation(const SEMListTransform *);       \
  void sem_list_system_##Label##_set_scale(const SEMListTransform *);

// Mutator (used in gizmo)
#define SEM_GENERIC_MUTATOR(Type, Label)                                       \
  void sem_system_##Label##_set_position(const SEMTransform *);                \
  void sem_system_##Label##_set_rotation(const SEMTransform *);                \
  void sem_system_##Label##_set_scale(const SEMTransform *);

// === Camera & Lights ===
SEM_GENERIC_ITEMS(SEM_LIST_GENERIC_MUTATOR);
SEM_GENERIC_ITEMS(SEM_GENERIC_MUTATOR);

SEM_LIGHT_ITEMS(SEM_LIST_GENERIC_MUTATOR);
SEM_LIGHT_ITEMS(SEM_GENERIC_MUTATOR);

SEM_SHADOW_ITEMS(SEM_LIST_GENERIC_MUTATOR);
SEM_SHADOW_ITEMS(SEM_GENERIC_MUTATOR);

// === Probe Reflection Grid ===
void sem_system_probe_reflection_grid_bound_set_position(
    const SEMTransform *);
void sem_system_probe_reflection_grid_bound_set_scale(
    const SEMTransform *);

void sem_system_probe_reflection_grid_set_position(const SEMTransform *);
void sem_system_probe_reflection_grid_set_rotation(const SEMTransform *);
void sem_system_probe_reflection_grid_set_scale(const SEMTransform *);

// === Plane Reflection ===
void sem_system_probe_reflection_plane_update_mesh_uniform(
    const SEMTransform *);

void sem_system_probe_reflection_plane_set_position(const SEMTransform *);
void sem_system_probe_reflection_plane_set_rotation(const SEMTransform *);
void sem_system_probe_reflection_plane_set_scale(const SEMTransform *);

#endif
