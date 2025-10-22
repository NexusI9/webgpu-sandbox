#ifndef _GIZMO_CORE_H_
#define _GIZMO_CORE_H_

#include <cglm/types.h>
#include <stddef.h>
#include <stdint.h>
#include <webgpu/webgpu.h>

#include "backend/ssbo.h"
#include "runtime/camera/camera.h"
#include "runtime/camera/core.h"
#include "runtime/geometry/plane/core.h"
#include "runtime/input/keyrecord.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/list.h"
#include "runtime/mesh/mesh.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/viewport/core.h"
#include "runtime/viewport/viewport.h"
#include "utils/color.h"
#include "utils/vector/core.h"
#include "utils/vector/vec3_list.h"
#include "utils/vector/vector.h"

#define GIZMO_SIZE 15.0f
#define GIZMO_POSITION_CAPACITY 128
#define GIZMO_AXIS_COUNT 3
#define GIZMO_MODE_COUNT 3

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

typedef struct Gizmo Gizmo;

typedef void (*gizmo_transform_callback)(Gizmo *, Camera *, Viewport *, vec3 *);

typedef enum {
  GizmoMode_Position,
  GizmoMode_Rotation,
  GizmoMode_Scale,
} GizmoMode;

typedef enum {
  GizmoSpace_Global,
  GizmoSpace_Local,
} GizmoSpace;

struct Gizmo {

  GizmoMode mode;
  GizmoSpace space;

  Axis axis;

  /**
     Visual Gizmo handles (mesh*) mostly use to hide/show targeted mesh based on
     gizmo mode
   */
  MeshRefList handles[GIZMO_AXIS_COUNT];

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
  MeshRefList interactive_handles[GIZMO_AXIS_COUNT];

  gizmo_transform_callback transform_callback[GIZMO_MODE_COUNT];

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

typedef struct {

  Camera *camera;
  Viewport *viewport;
  MeshList *list; // mesh pool from which gizmo mesh will be created
} GizmoCreateDescriptor;

static const color *gizmo_handle_color[6] = {
    // 1D
    &COLOR_GIZMO_X,
    &COLOR_GIZMO_Y,
    &COLOR_GIZMO_Z,
    // 2D
    &COLOR_GIZMO_XY,
    &COLOR_GIZMO_YZ,
    &COLOR_GIZMO_XZ,
};

typedef void (*gizmo_create_handles_callback)(MeshRefList *, MeshRefList *,
                                              const GizmoCreateDescriptor *);

EXTERN_C_BEGIN
void gizmo_create(Gizmo *, const GizmoCreateDescriptor *desc);


void gizmo_update_mode(Gizmo *, MeshRefList *, GizmoMode);

void gizmo_remove(Gizmo *, MeshRefList *);

void gizmo_set_position(Gizmo *, vec3);
void gizmo_set_rotation(Gizmo *, vec3);

void gizmo_set_active(Gizmo *, Camera *, Viewport *);

void gizmo_set_axis_from_mesh(Gizmo *, const Mesh *);

/**
   Clear gizmo cached data. Used on HTML events mouse up so
   during the next mouse down we can repopulate the new data.
 */
static inline void gizmo_clear_active(Gizmo *gizmo) {
  // reset gizmo initial position and delta
  glm_vec3_copy(GLM_VEC3_ZERO, gizmo->cache.gizmo_init_position);
  glm_vec3_copy(GLM_VEC3_ZERO, gizmo->cache.init_delta);
  gizmo->cache.init_distance = 0.0f;
}

/**
   Got through the active meshes and update their uniform back to their default
   one. Function primarily used in the selection callback to set back the handle
   color on mouse leave.

   Use a lookup table coupled with a linear search to pick the right pointer.
 */
static inline void gizmo_reset_color_uniform(Gizmo *gizmo) {
  for (uint8_t i = 0; i < gizmo->interactive_handles[gizmo->mode].length; i++) {
    Mesh *handle = gizmo->interactive_handles[gizmo->mode].entries[i];
    shader_update_uniform_data(mesh_shader(handle, MeshShader_Fixed), 1, 0,
                               (void *)gizmo_handle_color[i % 6],
                               ShaderUpdateFlag_None);
  }
}

static inline void gizmo_update_ssbo(Gizmo *gizmo, SSBOManager *ssbo) {
  for (uint8_t i = 0; i < gizmo->handles[gizmo->mode].length; i++)
    ssbo_update_queue_insert(
        ssbo, SSBOType_Mesh,
        gizmo->handles[gizmo->mode].entries[i]->ssbo_slot.id);
}

EXTERN_C_END

#endif
