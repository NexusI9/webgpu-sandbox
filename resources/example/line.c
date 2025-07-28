#include "line.h"

#include "../../runtime/geometry/line/line.h"

void example_line(Scene *scene) {
  Mesh *line = scene_new_mesh(scene);
  line_create(line, &(LineCreateDescriptor){
                        .device = scene_device(scene),
                        .queue = scene_queue(scene),
                        .name = "line mesh",
                    });

  line_add_point((vec3){-2.0f, -4.0f, -2.0f}, (vec3){2.0f, 4.0f, 2.0f},
                 (vec3){1.0f, 1.0f, 1.0f}, &line->topology.base.attribute,
                 &line->topology.base.index);

  line_add_point((vec3){3.0f, -2.0f, -2.0f}, (vec3){-3.0f, 7.0f, 3.0f},
                 (vec3){0.0f, 1.0f, 0.0f}, &line->topology.base.attribute,
                 &line->topology.base.index);

  scene_add_mesh(scene, line, ScenePipeline_Dynamic_Unlit, NULL);
}
