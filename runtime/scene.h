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

typedef uint8_t shader_bind_t;

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
  MeshIndexedList fixed;
} SceneLayerList;


typedef struct {
  PointLightList point;
  SpotLightList spot;
  AmbientLightList ambient;
  SunLightList sun;
} SceneLightList;

typedef struct {

  // camera
  Camera camera;

  // viewport
  Viewport viewport;

  // meshes global list
  MeshList meshes;

  // meshes layers
  SceneLayerList layer;

  // light type
  SceneLightList lights;

} Scene;

typedef void (*scene_draw_callback)(Scene *, WGPURenderPassEncoder *);
typedef void (*scene_build_callback)(Scene *);

Scene scene_create(Camera, Viewport);

// dynamic rendering
Mesh *scene_new_mesh_lit(Scene *);
Mesh *scene_new_mesh_unlit(Scene *);

void scene_build_texture(Scene *);
void scene_build_shadow(Scene *);
void scene_build_solid(Scene *);
void scene_build_wireframe(Scene *);

void scene_draw_texture(Scene *, WGPURenderPassEncoder *);
void scene_draw_shadow(Scene *, WGPURenderPassEncoder *);
void scene_draw_solid(Scene *, WGPURenderPassEncoder *);
void scene_draw_wireframe(Scene *, WGPURenderPassEncoder *);

// fixed rendering
Mesh *scene_new_mesh_fixed(Scene *);

void scene_build_fixed(Scene *);

void scene_draw_fixed(Scene *, WGPURenderPassEncoder *);


// light
size_t scene_add_point_light(Scene *, PointLightDescriptor *);
size_t scene_add_spot_light(Scene *, SpotLightDescriptor *);
size_t scene_add_ambient_light(Scene *, AmbientLightDescriptor *);
size_t scene_add_sun_light(Scene *, SunLightDescriptor *);

#endif
