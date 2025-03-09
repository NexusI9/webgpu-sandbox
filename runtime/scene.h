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

  // meshes
  mesh_list mesh_list;

} scene;

scene scene_create(camera, viewport);
mesh *scene_add_mesh(scene *, mesh *);
void scene_draw(scene *, WGPURenderPassEncoder *);

#endif
