#ifndef _GIZMO_TRANSFORM_CORE_H_
#define _GIZMO_TRANSFORM_CORE_H_

#include "../../mesh/mesh.h"
#include "../core.h"
#include "../utils/vector/vector.h"
#include <stddef.h>

#define GIZMO_TRANSFORM_SIZE 15.0f
#define GIZMO_TRANSFORM_POSITION_CAPACITY 128
#define GIZMO_TRANSFORM_AXIS_COUNT 3

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
                       |     HTML EVENTS (clicks...)      |
                       |      if {...} else {...}         |
                       '----------------------------------'
                                  Push ⎜ Pop
                                       ▼
    .-- Cache Selection Array --------------------------------------------.
    | hit mesh*  |  hit mesh*  |  hit mesh*  |         |         |        |
    '---------------------------------------------------------------------'
                                       ⎜
                                       ▼
                        .--------------------------------.
                        |    -------------------------.  |
                        |  ▲      Transform loop      ▼  |
                        |  '--------------------------   |
                        '--------------------------------'
 */

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

  /**
     Gizmo handles (mesh*) mostly use to hide/show targeted mesh based on gizmo
     mode
   */
  MeshRefList handles[GIZMO_TRANSFORM_AXIS_COUNT];

  /**
     TODO:
     Temporarily need to separate Visual handles from Interactive ones cause the
     rotate gizmo use a sphere in the middle as occluder.
     However since we only implemented the the "AABB" boundind box model, the
     occluder boundbox conflicts with the actual gizmo axis boundbox and cancel
     the axis selection based on which handle has been clicked on.

     tl;dr: raycast basically always detects the occluder cause its hitbox is
     bigger.

     The temporal solution is to include in a separate list the interactive
     handles.

     A more robust solution to this is to set a Hull boundbox around the
     occluder so is doesn't override the other handle. (Yet to be
     implemented...)
   */
  MeshRefList interactive_handles[GIZMO_TRANSFORM_AXIS_COUNT];

  gizmo_transform_callback transform_callback[GIZMO_TRANSFORM_AXIS_COUNT];

  /**
     Cached attribute on click
   */
  struct {
    MeshRefList selection;
    Vec3List selection_init_attribute;
    vec3 delta_init;
    InfinitePlane plane;
    vec3 gizmo_init_position;
  } cache;

  /**
     Keyboard inputs state
   */
  KeyRecord keys;
};

typedef void (*gizmo_transform_create_handles_callback)(
    MeshRefList *, MeshRefList *, const GizmoCreateDescriptor *);

void gizmo_transform_create(GizmoTransform *,
                            const GizmoCreateDescriptor *desc);

void gizmo_transform_update_mode(GizmoTransform *, MeshRefList *,
                                 GizmoTransformMode);

void gizmo_transform_remove(GizmoTransform *, MeshRefList *);

void gizmo_transform_translate(GizmoTransform *, vec3);
void gizmo_transform_rotate(GizmoTransform *, vec3);

void gizmo_transform_set_active(GizmoTransform *, const Mesh *,
                                const MeshRefList *, Camera *, Viewport *);

void gizmo_transform_clear_active(GizmoTransform *);

#endif
