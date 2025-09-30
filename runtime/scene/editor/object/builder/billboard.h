#ifndef _SCENE_EDITOR_OBJECT_BUILDER_BILLBOARD_H_
#define _SCENE_EDITOR_OBJECT_BUILDER_BILLBOARD_H_

#include "runtime/scene/core.h"
#define SEO_BILLBOARD_SCALE ((vec3){0.85f, 0.85f, 0.85f})

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
  const char *texture_path;
} SEOCreateBillboardDescriptor;

void seo_create_billboard(Mesh *, const SEOCreateBillboardDescriptor *);

void seo_billboard_highlight_callback(const SEOHighlightCallback *);
#endif
