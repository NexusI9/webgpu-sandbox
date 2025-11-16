#include "child.h"

#include <cglm/types.h>

#include "backend/renderer/core.h"
#include "backend/std_pipeline/core.h"
#include "primitive.h"
#include "runtime/mesh/core.h"
#include "runtime/pipeline/render.h"

void example_child(Engine *engine) {

  Scene *scene = engine_get_active_scene(engine);

  RenderPipeline *const *pipeline =
      std_render_pipeline(RenderPipelineType_Default);

  Mesh *parent_cube;
  example_primitive(engine, parent_cube, (vec3){3.0f, 2.0f, 1.0f}, pipeline);
  Mesh *child_cube_A, *child_cube_B;
  example_primitive(engine, child_cube_A, (vec3){-4.0f, -2.0f, -1.0f},
                    pipeline);
  example_primitive(engine, child_cube_B, (vec3){-3.0f, -9.0f, 1.0f}, pipeline);

  mesh_child_add(parent_cube, child_cube_A);
  mesh_child_add(parent_cube, child_cube_B);
}
