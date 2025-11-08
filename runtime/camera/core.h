#ifndef _CAMERA_CORE_H_
#define _CAMERA_CORE_H_

#define CAMERA_MODE_COUNT 4

#include <cglm/cglm.h>
#include <cglm/types.h>
#include <emscripten/html5.h>
#include <stddef.h>
#include <stdint.h>

#include "backend/clock.h"
#include "backend/registry.h"
#include "backend/ubo.h"

typedef struct Camera Camera;

typedef enum {
  CameraStatus_Success,
  CameraStatus_AllocFail,
  CameraStatus_Error,
} CameraStatus;

typedef enum {
  CameraMode_Fixed,
  CameraMode_Flying,
  CameraMode_Orbit,
  CameraMode_Edit
} CameraMode;

typedef struct {
  Camera **entries;
  size_t length;
  size_t capacity;
} CameraList;

typedef struct {
  float move;
  float rotate;
  float zoom;
} CameraSensitivity;

typedef struct {
  float speed;
  CameraMode mode;
  CameraSensitivity sensitivity;
} CameraCreateDescriptor;

typedef struct {
  mat4 view;
  vec4 position;
  vec4 lookat;
  uint32_t mode;
  uint32_t _pad[39];
} CameraUniform;

struct Camera {

  reg_id_t id;

  vec3 position;
  vec3 euler_rotation;
  vec3 target;
  vec3 forward;
  vec3 up;
  vec3 right;

  UBOSlot ubo_slot;

  mat4 view;

  float speed;
  CameraSensitivity sensitivity;

  CameraMode mode;
};

void camera_create(Camera *, const CameraCreateDescriptor *);
void camera_reset(Camera *);
void camera_lookat(Camera *, const vec3, const vec3);

// get
float camera_position(const Camera *);
float camera_euler_rotation(const Camera *);
mat4 *camera_view(Camera *);

// set
void camera_set_position(Camera *, const vec3);
void camera_set_rotation(Camera *, const vec3);
void camera_update_view(Camera *);

void camera_destroy(Camera *);
#endif
