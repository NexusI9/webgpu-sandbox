#ifndef _SCENE_EDITOR_OBJECT_BUILDER_BILLBOARD_H_
#define _SCENE_EDITOR_OBJECT_BUILDER_BILLBOARD_H_

#include "runtime/scene/core.h"
#define SEM_BILLBOARD_SCALE ((vec3){0.85f, 0.85f, 0.85f})

#include <cglm/cglm.h>
#include <cglm/types.h>
#include <stddef.h>
#include <webgpu/webgpu.h>

#include "runtime/mesh/core.h"
#include "runtime/mesh/mesh.h"

typedef struct {
  vec3 *position;
  vec3 *scale;

  const char *label;
  const WGPUTextureView view;
  const vec2 uv0, uv1;
} SEMCreateBillboardDescriptor;

void sem_create_billboard(Mesh *, const SEMCreateBillboardDescriptor *);

void sem_billboard_select_callback(const SEMHighlightCallback *);
void sem_billboard_deselect_callback(const SEMHighlightCallback *);
#endif
