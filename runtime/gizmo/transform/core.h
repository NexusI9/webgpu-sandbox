#ifndef _GIZMO_TRANSFORM_CORE_H_
#define _GIZMO_TRANSFORM_CORE_H_

#include "../../mesh/mesh.h"
#include "../core.h"
#include "../utils/vector/vector.h"
#include "translate.h"
#include <stddef.h>

#define GIZMO_TRANSFORM_SIZE 15.0f
#define GIZMO_TRANSFORM_POSITION_CAPACITY 128

typedef struct GizmoTransform GizmoTransform;

typedef void (*gizmo_transform_callback)(GizmoTransform *, Camera *,
                                         Viewport *);

typedef enum {
  GizmoTransformMode_Translate,
  GizmoTransformMode_Rotate,
  GizmoTransformMode_Scale,
} GizmoTransformMode;

struct GizmoTransform {

  GizmoTransformMode mode;

  Axis axis;
  MeshRefList handles[3];
  gizmo_transform_callback transform_callback[3];

  // cached attribute on click
  /**
    Gizmo Transform

    We poll mouse event to ensure smooth movement of the meshes.
    Gizmo transform callback use data oriented pattern to prevent ifs and jumps
    within the loop callback.

    To do so we split it in two parts:

    1.[COLD PATH] The HTML Event: that takes care to gather the data and
    populate the selection array. a. On Right click: add mesh to cached
    selection array. b. On Left click: cache selection and gizmo initial
    positions.

    2.[HOT PATH] The Transform callback loop (called during main update) : This
    callback simply traverse the cache selection and apply the same callback to
    all cached meshes.

    Having the ifs and logic conditions in the HTML event, then populating an
    array that's being travered in the loop prevents ifs/ last minute decision
    on the hot path.

                         .----------------------------------.
                         |                                  |
                         |     HTML EVENTS (clicks...)      |
                         |                                  |
                         '----------------------------------'
                                    push ⎜ Pop
                                         ▼
      .----- Cache Selection Array -----------------------------------------.
      | gizmo handles* | hit mesh* | hit mesh* |        |         |         |
      '---------------------------------------------------------------------'
                                         ⎜
                                         ▼
                          .--------------------------------.
                          |    -------------------------.  |
                          |  ▲      Transform loop      ▼  |
                          |  '--------------------------   |
                          '--------------------------------'
  */
  struct {
    MeshRefList selection;
    Vec3List selection_init_positions;
    vec3 gizmo_init_position;
  } cache;
};

void gizmo_transform_create(GizmoTransform *,
                            const GizmoCreateDescriptor *desc);

void gizmo_transform_update_mode(GizmoTransform *, MeshRefList *,
                                 GizmoTransformMode);

void gizmo_transform_remove(GizmoTransform *, MeshRefList *);

void gizmo_transform_translate(GizmoTransform *, vec3);
void gizmo_transform_translate_add(GizmoTransform *, float, const Axis);
void gizmo_transform_rotate_add(GizmoTransform *, float, const Axis);

/**
   TODO: Currently we use the active handle as a switch (boolean/flag-like
   approach) to detect if the loop callback should actually move the meshes
   based on mouse position.

   Maybe find a more data-oriented-friendly way for this approach.
 */
void gizmo_transform_set_active(GizmoTransform *, const Mesh *,
                                const MeshRefList *, Camera *, Viewport *);

void gizmo_transform_clear_active(GizmoTransform *);

#endif
