#ifndef _SCENE_H_
#define _SCENE_H_

#include "camera.h"
#include "mesh.h"
#include "viewport.h"
#include "webgpu/webgpu.h"

#define SCENE_MESH_LIST_DEFAULT_CAPACITY 32

typedef struct {

  // camera
  camera camera;

  // viewport
  viewport viewport;

  // meshes layers
  // due to depth test, need to write fully solid mesh first and then
  // transparent meshes
  struct {
    mesh_list solid;
    mesh_list alpha;
  } meshes;

} scene;

scene scene_create(camera, viewport);
mesh *scene_add_mesh_solid(scene *, mesh *);
mesh *scene_add_mesh_alpha(scene *, mesh *);
void scene_draw(scene *, WGPURenderPassEncoder *);

#endif
