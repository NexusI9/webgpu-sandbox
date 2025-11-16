#ifndef _SCENE_EDITOR_OBJECT_BUILDER_WIREFRAME_H_
#define _SCENE_EDITOR_OBJECT_BUILDER_WIREFRAME_H_
#include <webgpu/webgpu.h>

#include "runtime/geometry/vertex/attribute.h"
#include "runtime/geometry/vertex/index.h"
#include "runtime/mesh/core.h"
#include "runtime/scene/core.h"
#include "utils/color.h"

#define SEM_WIREFRAME_LINE_THICKNESS 0.001f

typedef struct {
  VertexAttribute *vertex;
  VertexIndex *index;
  color *color;
  float thickness;
  const char *name;
} SEMCreateWireframeDescriptor;

void sem_create_wireframe(Mesh *, const SEMCreateWireframeDescriptor *);
void sem_wireframe_select_callback(SEMHighlightCallback *);
void sem_wireframe_deselect_callback(SEMHighlightCallback *);

#endif
