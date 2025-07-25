#ifndef _GIZMO_TRANSFORM_CORE_H_
#define _GIZMO_TRANSFORM_CORE_H_

#include "../../mesh/mesh.h"
#include "../core.h"
#include "../utils/vector.h"
#include "translate.h"

#define GIZMO_TRANSFORM_SIZE 15.0f

typedef struct GizmoTransform GizmoTransform;

typedef void (*gizmo_transform_callback)(GizmoTransform *, MeshRefList *,
                                         Camera *, Viewport *);

typedef enum {
  GizmoTransformMode_Translate,
  GizmoTransformMode_Rotate,
  GizmoTransformMode_Scale,
} GizmoTransformMode;

struct GizmoTransform {
  GizmoTransformMode mode;
  vec3 init_offset;
  Axis axis;
  MeshRefList *active_handle;
  MeshRefList handles[3];
  gizmo_transform_callback transform_callback[3];
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
void gizmo_transform_set_active(GizmoTransform *, const Mesh *, Camera *,
                                Viewport *);

void gizmo_transform_clear_active(GizmoTransform *);

#endif
