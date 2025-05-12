
#ifndef _SCENE_H_
#define _SCENE_H_

#include "camera.h"
#include "light.h"
#include "mesh.h"
#include "viewport.h"
#include "webgpu/webgpu.h"
#include <stddef.h>

#define SCENE_MESH_LIST_DEFAULT_CAPACITY 32
#define SCENE_MESH_MAX_MESH_CAPACITY 64

// due to depth test, need to write fully solid mesh first and then
// transparent meshes

/*
  Scene has a global list of mesh and sublist of mesh pointers that are called
  during certain render pass.
  Nothe that the meshes children also holds pointers to this global list, hence
  it's necessary to take care to handle them accordingly if a mesh is added or
  removed from the global list.

       Global List (Pool)        Pass Lists

                                 [Lit/ Physical Meshes]
                                 .----------.
                       .-------> | 0x3948ef |
                      |	         |----------|
       .----------.   |  .-----> | 0x49da39 |
       |  Mesh 1  | --' |        |----------|
       |----------|     |  .-->  | 0xed93fa |
       |  Mesh 3  | ----' |      '----------'
       |----------|       |
       |  Mesh 4  | ------'
       |----------|
       |  Mesh 5  | ------.
       |----------|       |     [Unlit/ Flat Meshes]
       |  Mesh 6  | ----. |     .----------.
       |----------|     |  '--> | 0x48daec |
       |  Mesh 7  | --. |       |----------|
       '----------'   | '-----> | 0x7423bc |
                      |         |----------|
                      '-------> | 0x3e2baf |
                                '----------'
 */
typedef struct {
  MeshIndexedList lit;
  MeshIndexedList unlit;
  MeshIndexedList gizmo;
} SceneLayerList;

typedef struct {
  mesh *entries;
  size_t capacity;
  size_t length;
} SceneMeshList;

typedef struct {
  PointLightList point;
  SpotLightList spot;
  AmbientLightList ambient;
  SunLightList sun;
} SceneLightList;

typedef struct {

  // camera
  camera camera;

  // viewport
  viewport viewport;

  // meshes global list
  SceneMeshList meshes;

  // meshes layers
  SceneLayerList layer;

  // light type
  SceneLightList lights;

} scene;

typedef void (*scene_draw_callback)(scene *, WGPURenderPassEncoder *);
typedef void (*scene_build_callback)(scene *);

scene scene_create(camera, viewport);
mesh *scene_new_mesh_lit(scene *);
mesh *scene_new_mesh_unlit(scene *);
mesh *scene_new_mesh_gizmo(scene *);

void scene_draw_texture(scene *, WGPURenderPassEncoder *);
void scene_draw_shadow(scene *, WGPURenderPassEncoder *);
void scene_draw_solid(scene *, WGPURenderPassEncoder *);
void scene_draw_wireframe(scene *, WGPURenderPassEncoder *);

void scene_build_texture(scene *);
void scene_build_shadow(scene *);
void scene_build_solid(scene *);
void scene_build_wireframe(scene *);

size_t scene_add_point_light(scene *, PointLightDescriptor *);
size_t scene_add_spot_light(scene *, SpotLightDescriptor *);
size_t scene_add_ambient_light(scene *, AmbientLightDescriptor *);
size_t scene_add_sun_light(scene *, SunLightDescriptor *);

#endif
