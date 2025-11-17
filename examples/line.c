#include "line.h"

#include "../include/resource_manager.h"
#include "../include/geometry.h"


void example_line(Engine *engine) {
  Mesh *line = rem_new_mesh();
  line_create(line, &(LineCreateDescriptor){
                        .name = "line mesh",
                    });

  line_add_point((vec3){-2.0f, -4.0f, -2.0f}, (vec3){2.0f, 4.0f, 2.0f},
                 (vec3){1.0f, 1.0f, 1.0f}, &line->topology.base.attribute,
                 &line->topology.base.index);

  line_add_point((vec3){3.0f, -2.0f, -2.0f}, (vec3){-3.0f, 7.0f, 3.0f},
                 (vec3){0.0f, 1.0f, 0.0f}, &line->topology.base.attribute,
                 &line->topology.base.index);

  engine_scene_add_mesh(engine, line, NULL,
                        EngineAddFlag_Unselectable | EngineAddFlag_TreeHide);
}
