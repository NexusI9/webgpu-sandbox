#ifndef _GIZMO_CAMERA_H_
#define _GIZMO_CAMERA_H_

#include "../runtime/camera.h"
#include "../runtime/mesh.h"
#include "./gizmo.h"

typedef struct {
  Camera *target;
  MeshRefList meshes;
} GizmoCamera;

void gizmo_camera_create(GizmoCamera *, Camera *,
                         const GizmoCreateDescriptor *);
void gizmo_camera_translate(GizmoCamera *, vec3);
void gizmo_camera_rotate(GizmoCamera *, vec3);
void gizmo_camera_scale(GizmoCamera *, vec3);
void gizmo_camera_lookat(GizmoCamera *, vec3);

#endif
