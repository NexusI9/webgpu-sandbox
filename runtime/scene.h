#ifndef _SCENE_H_
#define _SCENE_H_

#include "camera.h"
#include "light.h"
#include "mesh.h"
#include "viewport.h"
#include "webgpu/webgpu.h"

#define SCENE_MESH_LIST_DEFAULT_CAPACITY 32

// due to depth test, need to write fully solid mesh first and then
// transparent meshes
typedef struct {
  MeshList solid;
  MeshList alpha;
} SceneMeshList;

typedef struct {
  PointLightList point;
  DirectionalLightList directional;
  AmbientLightList ambient;
} SceneLightList;

typedef struct {

  // camera
  camera camera;

  // viewport
  viewport viewport;

  // meshes layers
  SceneMeshList meshes;

  // light type
  SceneLightList lights;

} scene;

scene scene_create(camera, viewport);
mesh *scene_add_mesh_solid(scene *, mesh *);
mesh *scene_add_mesh_alpha(scene *, mesh *);
void scene_draw(scene *, MeshDrawMethod, WGPURenderPassEncoder *);
void scene_build(scene *, MeshDrawMethod);

size_t scene_add_point_light(scene *, PointLightDescriptor *);
size_t scene_add_directional_light(scene *, DirectionalLightDescriptor *);
size_t scene_add_ambient_light(scene *, AmbientLightDescriptor *);

#endif
