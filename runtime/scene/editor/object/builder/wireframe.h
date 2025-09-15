#ifndef _SCENE_EDITOR_OBJECT_BUILDER_WIREFRAME_H_
#define _SCENE_EDITOR_OBJECT_BUILDER_WIREFRAME_H_
#include "../runtime/scene/core.h"

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
