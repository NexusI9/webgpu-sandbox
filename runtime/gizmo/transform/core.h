#ifndef _GIZMO_TRANSFORM_CORE_H_
#define _GIZMO_TRANSFORM_CORE_H_

#include "../../mesh/mesh.h"
#include "../core.h"
#include "../utils/vector/vector.h"
#include <stddef.h>

#define GIZMO_TRANSFORM_SIZE 15.0f
#define GIZMO_TRANSFORM_POSITION_CAPACITY 128

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

typedef struct {
  Mesh *mesh[3];
  Axis axis[3];
} GizmoTransformMeshAxis;

struct GizmoTransform {

  GizmoTransformMode mode;

  Axis axis;

  /**
     Gizmo handles (mesh*) mostly use to hide/show targeted mesh based on gizmo
     mode
   */
  MeshRefList handles[3];

  /**
     Handles axis axes cached mesh pointers that will be used later to retrieve
     the selected axis based on the clicked mesh pointer.

     Mesh : [0x30443] , [0x45832] , [0x95943]
               |            |           |
     Axis : [Axis_X]  , [Axis_Y]  ,  [Axis_Z]

   */
  GizmoTransformMeshAxis handles_axis[3];

  gizmo_transform_callback transform_callback[3];

  /**
     Cached attribute on click
   */
  struct {
    MeshRefList selection;
    Vec3List selection_init_attribute;
    vec3 delta_init;
    vec3 gizmo_init_position;
  } cache;
};

typedef void (*gizmo_transform_create_handles_callback)(
    MeshRefList *, GizmoTransformMeshAxis *, const GizmoCreateDescriptor *);

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
