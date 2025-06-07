#include "line.h"

void example_line(Scene *scene, renderer *renderer) {
  Mesh *line = scene_new_mesh_unlit(scene);
  line_create(line, &(LineCreateDescriptor){
                        .device = renderer_device(renderer),
                        .queue = renderer_queue(renderer),
                        .name = "line mesh",
                    });

  line_add_point((vec3){-2.0f, -4.0f, -2.0f}, (vec3){2.0f, 4.0f, 2.0f},
                 (vec3){1.0f, 1.0f, 1.0f}, &line->topology.base.attribute,
                 &line->topology.base.index);

  line_add_point((vec3){3.0f, -2.0f, -2.0f}, (vec3){-3.0f, 7.0f, 3.0f},
                 (vec3){0.0f, 1.0f, 0.0f}, &line->topology.base.attribute,
                 &line->topology.base.index);
}
