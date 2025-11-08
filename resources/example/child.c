#include "child.h"

#include <cglm/types.h>

#include "backend/std_pipeline/core.h"
#include "primitive.h"
#include "runtime/mesh/core.h"
#include "runtime/pipeline/render.h"

void example_child(Scene *scene, Renderer *renderer) {

  const RenderPipeline *pipeline =
      std_render_pipeline(RenderPipelineType_Default);
  Mesh *parent_cube;
  example_primitive(parent_cube, (vec3){3.0f, 2.0f, 1.0f}, scene, renderer,
                    pipeline);
  Mesh *child_cube_A, *child_cube_B;
  example_primitive(child_cube_A, (vec3){-4.0f, -2.0f, -1.0f}, scene, renderer,
                    pipeline);
  example_primitive(child_cube_B, (vec3){-3.0f, -9.0f, 1.0f}, scene, renderer,
                    pipeline);

  mesh_child_add(parent_cube, child_cube_A);
  mesh_child_add(parent_cube, child_cube_B);
}
