#ifndef _GIZMO_H_
#define _GIZMO_H_

#include <cglm/cglm.h>
#include <stddef.h>
#include <webgpu/webgpu.h>

#include "./camera.h"
#include "./light.h"
#include "./mesh.h"

#define GIZMO_LIST_CAPACITY_DEFAULT 16
#define GIZMO_SUCCESS 0
#define GIZMO_ALLOC_FAIL 1
#define GIZMO_ERROR 2

typedef Mesh Gizmo;

typedef struct {
  PointLight *target;
  MeshRefList meshes;
} GizmoPointLight;

typedef struct {
  AmbientLight *target;
  MeshRefList meshes;
} GizmoAmbientLight;

typedef struct {
  SunLight *target;
  MeshRefList meshes;
} GizmoSunLight;

typedef struct {
  SpotLight *target;
  MeshRefList meshes;
} GizmoSpotLight;

typedef struct {
  Camera *target;
  MeshRefList meshes;
} GizmoCamera;

typedef struct {

  struct {
    size_t length;
    size_t capacity;
    GizmoPointLight *entries;
  } point_light;

  struct {
    size_t length;
    size_t capacity;
    GizmoSunLight *entries;
  } sun_light;

  struct {
    size_t length;
    size_t capacity;
    GizmoSpotLight *entries;
  } spot_light;

  struct {
    size_t length;
    size_t capacity;
    GizmoAmbientLight *entries;
  } ambient_light;

  struct {
    size_t length;
    size_t capacity;
    GizmoCamera *entries;
  } camera;

} GizmoList;

typedef struct {
  vec3 *position;
  vec3 *scale;
  WGPUDevice *device;
  WGPUQueue *queue;
  const char *label;
  const char *texture_path;
  Camera *camera;
  Viewport *viewport;
} GizmoCreateBillboardDescriptor;

typedef struct {
  void **entries;
  size_t *capacity;
  size_t *length;
  size_t type_size;
  void *new_entry;
} GizmoListInsertDescriptor;

typedef struct {
  void **entries;
  size_t *capacity;
  size_t *length;
  size_t type_size;
} GizmoListNewDescriptor;

void gizmo_create_billboard(Gizmo *, const GizmoCreateBillboardDescriptor *);
int gizmo_list_create(GizmoList *, size_t capacity);

// point light gizmo
int gizmo_list_insert_point_light(GizmoList *, PointLight *, MeshRefList *);
PointLight *gizmo_list_new_point_light(GizmoList *);

// ambient light gizmo
int gizmo_list_insert_ambient_light(GizmoList *, AmbientLight *, MeshRefList *);
AmbientLight *gizmo_list_new_ambient_light(GizmoList *);

// sun light gizmo
int gizmo_list_insert_sun_light(GizmoList *, SunLight *, MeshRefList *);
SunLight *gizmo_list_new_sun_light(GizmoList *);

// spot light gizmo
int gizmo_list_insert_spot_light(GizmoList *, SpotLight *, MeshRefList *);
SpotLight *gizmo_list_new_spot_light(GizmoList *);

// camera gizmo
int gizmo_list_insert_camera(GizmoList *, Camera *, MeshRefList *);
Camera *gizmo_list_new_camera(GizmoList *);
#endif
