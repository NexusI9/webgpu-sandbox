#include "ray.h"

#include <stdio.h>

#include "runtime/geometry/line/core.h"
#include "runtime/mesh/list.h"
#include "runtime/mesh/ref_list.h"
#include "runtime/mesh/shader/core.h"
#include "runtime/shader/update.h"
#include "utils/color.h"
#include "runtime/mesh/core.h"

void scene_debug_ray_create(SceneDebug *debug, Mesh **line) {
  *line = mesh_list_new_mesh(debug->pool);
  line_create(*line, &(LineCreateDescriptor){
                         .name = "Debug line mesh",
                     });

  mesh_shader_build_mvp(*line, MeshShader_Fixed, debug->ssbo);

  shader_update_uniform_data(mesh_shader(*line, MeshShader_Fixed), 1, 0,
                             &(color){0.0f, 1.0f, 0.0f, 1.0f});
}

void scene_debug_ray_add_point(Mesh *line, vec3 origin, vec3 target,
                               color color) {

  line_add_point(origin, target, color, &line->topology.base.attribute,
                 &line->topology.base.index);
}

void scene_debug_ray_build(SceneDebug *debug, Mesh *line) {
  line_update_buffer(line);
  mesh_ref_list_insert(&debug->object_list[SceneDebugObject_Ray], line);
  printf("debug ray length: %lu\n",
         debug->object_list[SceneDebugObject_Ray].length);
}
