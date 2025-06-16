#ifndef _CAMERA_CORE_H_
#define _CAMERA_CORE_H_

#define CAMERA_SUCCESS 0
#define CAMERA_ALLOC_FAIL 1
#define CAMERA_ERROR 2

#include "../backend/clock.h"

#include <cglm/cglm.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
  CameraMode_Fixed = 1 << 0,
  CameraMode_Flying = 1 << 1,
  CameraMode_Orbit = 1 << 2,
} CameraMode;

typedef struct {
  struct Camera *entries;
  size_t length;
  size_t capacity;
} CameraList;

typedef struct {
  cclock *clock;
  float speed;
  CameraMode mode;
  float sensitivity;
  float wheel_sensitivity;
} CameraCreateDescriptor;

typedef struct Camera {

  cclock *clock;

  vec3 position;
  vec3 euler_rotation;
  vec3 target;

  vec3 forward;
  vec3 up;
  vec3 right;

  mat4 view;

  float speed;
  float sensitivity;
  float wheel_sensitivity;
  CameraMode mode;

} Camera;

void camera_create(Camera *, const CameraCreateDescriptor *);
void camera_reset(Camera *);
void camera_set_mode(Camera *, CameraMode);
void camera_draw(Camera *);
void camera_lookat(Camera *, vec3, vec3);

// get
float camera_position(const Camera *);
float camera_euler_rotation(const Camera *);
mat4 *camera_view(Camera *);

// set
void camera_translate(Camera *, vec3);
void camera_rotate(Camera *, vec3);
void camera_update_view(Camera *);

#endif
