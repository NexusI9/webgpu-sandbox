#ifndef _SCENE_EDITOR_OBJECT_BUILDER_WIREFRAME_H_
#define _SCENE_EDITOR_OBJECT_BUILDER_WIREFRAME_H_
#include "../core.h"

#define SEO_WIREFRAME_LINE_THICKNESS 0.001f

typedef struct {
  WGPUDevice *device;
  WGPUQueue *queue;
  VertexAttribute *vertex;
  VertexIndex *index;
  vec3 *color;
  float thickness;
  const char *name;
} SEOCreateWireframeDescriptor;

void seo_create_wireframe(Mesh *, const SEOCreateWireframeDescriptor *);

#endif
