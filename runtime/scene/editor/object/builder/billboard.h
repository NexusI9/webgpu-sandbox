#ifndef _SCENE_EDITOR_OBJECT_BUILDER_BILLBOARD_H_
#define _SCENE_EDITOR_OBJECT_BUILDER_BILLBOARD_H_

#define SEO_BILLBOARD_SCALE ((vec3){0.85f, 0.85f, 0.85f})

#include <cglm/cglm.h>
#include <stddef.h>
#include <webgpu/webgpu.h>
#include <cglm/types.h>

#include "runtime/mesh/mesh.h"
#include "runtime/mesh/core.h"

typedef struct {
  vec3 *position;
  vec3 *scale;
  const WGPUDevice device;
  const WGPUQueue queue;
  const char *label;
  const char *texture_path;
} SEOCreateBillboardDescriptor;

void seo_create_billboard(Mesh *, const SEOCreateBillboardDescriptor *);

#endif
