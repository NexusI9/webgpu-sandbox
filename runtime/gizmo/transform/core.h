#ifndef _GIZMO_TRANSFORM_CORE_H_
#define _GIZMO_TRANSFORM_CORE_H_

#include "../../mesh/mesh.h"
#include "../core.h"
#include "../utils/vector/vector.h"
#include <stddef.h>

#define GIZMO_TRANSFORM_SIZE 15.0f
#define GIZMO_TRANSFORM_POSITION_CAPACITY 128
#define GIZMO_TRANSFORM_AXIS_COUNT 3
#define GIZMO_TRANSFORM_MODE_COUNT 3

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
   Cold Path           |     HTML EVENTS (clicks...)      |
   (jumps/conds)       |      if {...} else {...}         |
                       '----------------------------------'
                                  Push ⎜ Pop
                                       ▼
    .-- Cache Selection Array --------------------------------------------.
    | hit mesh*  |  hit mesh*  |  hit mesh*  |         |         |        |
    '---------------------------------------------------------------------'
                                       ⎜
                                       ▼
                        .--------------------------------.
    Hot path            |    -------------------------.  |
    (array traverse)    |  ▲      Transform loop      ▼  |
                        |  '--------------------------   |
                        '--------------------------------'

    ---------------------------------------------------------------------------
    ---------------------------------------------------------------------------

                       *  Gizmo Transform X Selection System *

    A. Selection Side

      .-------------------.
      | Add Mesh to Scene |
      |  Selection List   |                    HTML EVENTS
      '-------------------'
                |            .----------------------. .-----------------------.
                |            |    Click on Handle   | |     Press hotkey      |
                |            '----------.-----------' '-----------.-----------'
                |                       |                         |
                |                       |             .-----------'-----------.
                |                       |             |   Hide/Show Handles   |
                |                       |             '-----------.-----------'
                |                       |                         |
                |           .-----------'-----------. .-----------'-----------.
                |           | Define Axis Direction | | Define Axis Direction |
                |           |    based on clicked   | |    based on hotkey    |
                |           |        handle         | |                       |
                |           '-----------.-----------' '------------.----------'
                |                       '------------.-------------'
   B. Gizmo     |                                    |
                |                   .----------------'-------------------.
                |                   |      ACTIVATE GIZMO (for loop)     |
                |                   |                                    |
                |                   |    .--------------------------.    |
                |                   |    |   Cache init attributes  |    |
                |                   |    '--------------------------'    |
                |                   |    .--------------------------.    |
                '----------------------> |  Transfert selection to  |    |
                                    |    |  Gizmo cached selection  |    |
                                    |    '------------.-------------'    |
                                    '---------------- | -----------------'
                                                      |
                                    .-----------------'------------------.
                                    |  -------------------------------.  |
                                    |  ▲    GIZMO TRANSFORM LOOP      ▼  |
                                    |  '------------------------------   |
                                    '------------------------------------'
 */

typedef struct GizmoTransform GizmoTransform;

typedef void (*gizmo_transform_callback)(GizmoTransform *, Camera *,
                                         Viewport *, vec3*);

typedef enum {
  GizmoTransformMode_Translate,
  GizmoTransformMode_Rotate,
  GizmoTransformMode_Scale,
} GizmoTransformMode;

typedef enum {
  GizmoTransformSpace_Global,
  GizmoTransformSpace_Local,
} GizmoTransformSpace;

struct GizmoTransform {

  GizmoTransformMode mode;
  GizmoTransformSpace space;

  Axis axis;

  /**
     Visual Gizmo handles (mesh*) mostly use to hide/show targeted mesh based on
     gizmo mode
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

  gizmo_transform_callback transform_callback[GIZMO_TRANSFORM_MODE_COUNT];

  /**
     Cached attribute on transform (click/ hotkey)

     - selection: act as a buffer between scene selection pipeline and
     selection draw loop. All meshes within the gizmo selection will be affected
     by the gizmo transformation callback.
     Meshes are only added from the scene selection to the gizmo when gizmo has
     been triggered either by clicking on the handles or by hotkey.
     The cache selection is updated during the "set_active" gizmo function.

     - selection_init_attribute: store each selection meshes their initial
     attributes depending on gizmo mode (loc/rot/scale) as to properly offset
     it.

     - init_delta: initial projected mouse position in space, used during
     transformation loop to properly offset.

     - init_distance: initial distance between the projected mouse to world
     space and the gizmo origin.

     - axis_direction: Overall direction according to which the transformation
     should operate.

     - plane: initial inifite plane from which normal is set depending on axis.
     Is used during 2D axis transformation (XY, YZ, XZ).

     - gizmo_init_position: used to project the mouse position to the closest
     point on an axis based on the gizmo position.

   */
  struct {
    MeshRefList selection;
    vec3 init_delta;
    float init_distance;
    float init_inv_distance;
    vec3 axis_direction;
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

void gizmo_transform_set_active(GizmoTransform *, MeshRefList **, size_t,
                                Camera *, Viewport *);

void gizmo_transform_clear_active(GizmoTransform *);

void gizmo_transform_set_axis_from_mesh(GizmoTransform *, const Mesh *);

#endif
