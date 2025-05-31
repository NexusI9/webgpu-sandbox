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
#define SCENE_CAMERA_LIST_CAPACITY 16

#define SCENE_SUCCESS 0
#define SCENE_MAX_CAPACITY_REACH 1
#define SCENE_ALLOC_FAILURE 2

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


   Render pass and Scene Mesh Lists work hand in hand.
   Meaning by pushing a mesh in a certain Scene Mesh List it will go through a
   predefined Renderer pipeline.

   The render passes are segmented in 2 global classes:
   - Dynamic: Will change depending on Render mode (wireframe/solid/textured).
   - Fixed: Is independant from Render mode.

   Currently the scene offers the following Mesh List depending on requirements:

   .----------.---------------.-----------.----------------.-------------------.
   |   Name   |  Shadow Pass  |  AO Pass  |  Fixed/Dynamic | Common use case   |
   |----------+---------------+-----------+----------------+-------------------|
   |   Lit    |       Y       |     Y     |     Dynamic    | Physical objects  |
   |----------+---------------+-----------+----------------+-------------------|
   |  UnLit   |        -      |     -     |     Dynamic    | Flat objects/ UI  |
   |----------+---------------+-----------+----------------+-------------------|
   |  Fixed   |        -      |     -     |      Fixed     | Gizmo/ Debug      |
   '----------'---------------'-----------'----------------'-------------------'

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
  CameraList cameras;
  Camera *active_camera;

  // viewport
  Viewport viewport;

  // meshes global list
  MeshList meshes;

  // meshes render layers
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
size_t scene_add_point_light(Scene *, PointLightDescriptor *, WGPUDevice *,
                             WGPUQueue *);
size_t scene_add_spot_light(Scene *, SpotLightDescriptor *, WGPUDevice *,
                            WGPUQueue *);
size_t scene_add_ambient_light(Scene *, AmbientLightDescriptor *, WGPUDevice *,
                               WGPUQueue *);
size_t scene_add_sun_light(Scene *, SunLightDescriptor *, WGPUDevice *,
                           WGPUQueue *);

MeshList *scene_mesh_list(Scene *);

#endif
