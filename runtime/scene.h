#ifndef _SCENE_H_
#define _SCENE_H_

#include "camera.h"
#include "mesh.h"
#include "viewport.h"
#include "webgpu/webgpu.h"

#define SCENE_MESH_LIST_DEFAULT_CAPACITY 32

typedef struct {
  mesh **items;
  uint8_t capacity;
  uint8_t length;
} mesh_list;

typedef struct {

  // camera
  camera camera;

  // viewport
  viewport viewport;

  // meshes
  mesh_list mesh_list;

} scene;

scene scene_create(camera, viewport);
void scene_add_mesh(scene *, mesh *);
void scene_draw(scene *, WGPURenderPassEncoder *);

#endif
