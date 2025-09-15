#ifndef _UTILS_VIEW_H_
#define _UTILS_VIEW_H_

#include <cglm/cglm.h>
#include <cglm/mat4.h>
#include <cglm/types.h>
#include <stddef.h>
#include <stdint.h>

#include "../runtime/camera/camera.h"
#include "../backend/ssbo.h"
#include "../runtime/camera/core.h"
#include "../runtime/viewport/core.h"
#include "../backend/ssbo.h"

#define PROJECTION_VIEW_COUNT 6
#define PROJECTION_SUN_DISTANCE 10

extern const vec3 projection_cubemaps_directions[PROJECTION_VIEW_COUNT];
extern const vec3 projection_cubemaps_ups[PROJECTION_VIEW_COUNT];

typedef struct {
  mat4 view;
  float _pad[48];
} ProjectionUniform;

typedef struct {
  mat4 projection;
  uint8_t length;
  mat4 views[PROJECTION_VIEW_COUNT];
  mat4 combined[PROJECTION_VIEW_COUNT];
} Projection;

// projections/view computing
void projection_mirror(Projection *, const vec3, const float, const Camera *,
                       const Viewport *);
void projection_point(Projection *, const vec3, const float, const float);
void projection_spot(Projection *, const vec3, const vec3, const float);
void projection_sun(Projection *, const vec3, const float);

/**
   Utils function that updates the slot according on the given projection.
   Since projections often work with an offset system, they requires multiple
   slots (especially point lights). As a result we also need to pass a field_id
   which represent the index of the starting slot.

   Using this function assumes that the slot uniform is of type
   ProjectionUniform.

   Function primarily used for lights and probes since they heavily rely on
   projections.
 */
static inline void projection_update_ssbo_slot(SSBOSlot *slot,
                                               Projection *views,
                                               size_t field_id) {
  for (uint8_t i = 0; i < views->length; i++) {
    ProjectionUniform uniform;
    glm_mat4_copy(views->combined[i], uniform.view);
    ssbo_slot_set_uniform(&slot[field_id + i], (void *)&uniform,
                          sizeof(ProjectionUniform));
  }
}

#endif
