#ifndef _SCENE_EDITOR_OBJECT_BUILDER_WIREFRAME_H_
#define _SCENE_EDITOR_OBJECT_BUILDER_WIREFRAME_H_
#include <webgpu/webgpu.h>

#include "runtime/scene/core.h"
#include "runtime/geometry/vertex/attribute.h"
#include "runtime/geometry/vertex/index.h"
#include "runtime/mesh/core.h"
#include "utils/color.h"

#define SEO_WIREFRAME_LINE_THICKNESS 0.001f

typedef struct {
  const WGPUDevice device;
  const WGPUQueue queue;
  VertexAttribute *vertex;
  VertexIndex *index;
  color *color;
  float thickness;
  const char *name;
} SEOCreateWireframeDescriptor;

void seo_create_wireframe(Mesh *, const SEOCreateWireframeDescriptor *);

#endif
