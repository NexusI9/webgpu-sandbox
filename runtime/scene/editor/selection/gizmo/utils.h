#ifndef _GIZMO_UTILS_H_
#define _GIZMO_UTILS_H_

#include <cglm/types.h>
#include <webgpu/webgpu.h>

#include "core.h"
#include "runtime/mesh/core.h"
#include "runtime/mesh/list.h"
#include "runtime/mesh/mesh.h"
#include "runtime/primitive/core.h"
#include "utils/color.h"

typedef struct {
  const char *mbin_path;
  MeshList *list;
  vec3 offset;
} GizmoCreateMeshDescriptor;

void gizmo_create_mesh(Mesh *, Primitive *, const color *);

void gizmo_create_handles(MeshRefList *, MeshRefList *,
                          const GizmoCreateMeshDescriptor *);

void gizmo_origin(Gizmo *, vec3 *);

#endif
