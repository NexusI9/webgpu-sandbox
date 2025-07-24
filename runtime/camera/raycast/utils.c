#include "utils.h"

/**
   Traverse all the mesh ref lists and determine if a mesh if found in it.
   Returns True if found. This skip the current mesh during the raycast hit test
   computation.
 */
bool camera_raycast_is_excluded(const MeshRefListArray *array, Mesh *mesh) {
  bool is_excluded = false;
  for (size_t i = 0; i < array->length; i++)
    for (size_t j = 0; j < array->lists[i]->length; j++)
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
  float dist = glm_vec3_distance(camera->position, mesh->position);
  float screen_scale = dist / scale;

  glm_vec3_scale(boundbox->min, screen_scale, boundbox->min);
  glm_vec3_scale(boundbox->max, screen_scale, boundbox->max);
}
