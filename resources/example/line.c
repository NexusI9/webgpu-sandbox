#include "line.h"

#include <cglm/types.h>
#include <stddef.h>

#include "backend/context.h"
#include "runtime/geometry/line/core.h"
#include "runtime/mesh/core.h"
#include "runtime/scene/add.h"
#include "runtime/scene/core.h"

void example_line(Scene *scene) {
  Mesh *line = scene_new_mesh(scene);
  line_create(line, &(LineCreateDescriptor){
                        .name = "line mesh",
                    });

  line_add_point((vec3){-2.0f, -4.0f, -2.0f}, (vec3){2.0f, 4.0f, 2.0f},
                 (vec3){1.0f, 1.0f, 1.0f}, &line->topology.base.attribute,
                 &line->topology.base.index);

  line_add_point((vec3){3.0f, -2.0f, -2.0f}, (vec3){-3.0f, 7.0f, 3.0f},
                 (vec3){0.0f, 1.0f, 0.0f}, &line->topology.base.attribute,
                 &line->topology.base.index);

  scene_add_mesh(scene, line, NULL,
                 SceneAddFlag_Unselectable | SceneAddFlag_TreeHide);
}
