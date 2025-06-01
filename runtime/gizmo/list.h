#ifndef _GIZMO_LIST_H_
#define _GIZMO_LIST_H_

#include "../camera.h"
#include "../light.h"
#include "../mesh.h"
#include "./camera.h"
#include "./light_ambient.h"
#include "./light_point.h"
#include "./light_spot.h"
#include "./light_sun.h"
#include <cglm/cglm.h>
#include <stddef.h>
#include <webgpu/webgpu.h>

#define GIZMO_LIST_CAPACITY_DEFAULT 16
#define GIZMO_LIST_SUCCESS 0
#define GIZMO_LIST_ALLOC_FAIL 1
#define GIZMO_LIST_ERROR 2

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
