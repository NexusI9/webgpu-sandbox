#include "utils.h"

#include <cglm/types.h>
#include <cglm/vec3.h>
#include <stddef.h>

/**
   Traverse all the mesh ref lists and determine if a mesh if found in it.
   Returns True if found. This skip the current mesh during the raycast hit test
   computation.
 */
bool camera_raycast_is_excluded(const MeshRefListArray *array, Mesh *mesh) {
  bool is_excluded = false;
  for (size_t i = 0; i < array->count; i++)
    for (size_t j = 0; j < array->lists[i]->count; j++)
      if (mesh == array->lists[i]->entries[j])
        is_excluded = true;

  return is_excluded;
}

/**
   Scale the boundbox so it matches the actual object size in screenspace.
   Mostly use for gizmos ray detection since they have a fixed size in the
   viewport.
 */
void camera_raycast_screen_space(Camera *camera, Mesh *mesh, float scale,
                                 AABB *boundbox) {

  vec3 cam_to_mesh;
  glm_vec3_sub(mesh->position, camera->position, cam_to_mesh);
  glm_vec3_normalize(cam_to_mesh);

  vec3 fixed_origin;
  glm_vec3_scale(cam_to_mesh, scale, fixed_origin);
  glm_vec3_add(camera->position, fixed_origin, fixed_origin);

  vec3 min_offset, max_offset;
  glm_vec3_sub(boundbox->min, mesh->position, min_offset);
  glm_vec3_sub(boundbox->max, mesh->position, max_offset);

  glm_vec3_add(fixed_origin, min_offset, boundbox->min);
  glm_vec3_add(fixed_origin, max_offset, boundbox->max);

}
