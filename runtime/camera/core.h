#ifndef _CAMERA_CORE_H_
#define _CAMERA_CORE_H_

#include "../backend/clock.h"
#include "../backend/registry.h"
#include <emscripten/html5.h>

#include <cglm/cglm.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
  CameraStatus_Success,
  CameraStatus_AllocFail,
  CameraStatus_Error,
} CameraStatus;

typedef enum {
  CameraMode_Fixed = 1 << 0,
  CameraMode_Flying = 1 << 1,
  CameraMode_Orbit = 1 << 2,
  CameraMode_Edit = 1 << 3,
} CameraMode;

typedef struct {
  struct Camera *entries;
  size_t length;
  size_t capacity;
} CameraList;

typedef struct {
  float move;
  float rotate;
  float zoom;
} CameraSensitivity;

typedef struct {
  cclock *clock;
  float speed;
  CameraMode mode;
  CameraSensitivity sensitivity;
} CameraCreateDescriptor;

typedef struct {
  mat4 view;
  vec4 position;
  vec4 lookat;
  uint32_t mode;
  uint32_t _pad[4];
} __attribute__((aligned(16))) CameraUniform;

typedef struct Camera {

  cclock *clock;
  id_t id;

  vec3 position;
  vec3 euler_rotation;
  vec3 target;
  vec3 forward;
  vec3 up;
  vec3 right;
  CameraUniform uniform;

  mat4 view;

  float speed;
  CameraSensitivity sensitivity;

  CameraMode mode;

} Camera;

void camera_create(Camera *, const CameraCreateDescriptor *);
void camera_reset(Camera *);
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
