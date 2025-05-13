#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "../backend/clock.h"

#include <cglm/cglm.h>
#include <stdint.h>

typedef enum {
  FIXED = 1 << 0,
  FLYING = 1 << 1,
  ORBIT = 1 << 2,
} CameraMode;

typedef struct {
  mat4 view;
  vec4 position;
  vec4 lookat;
  uint32_t mode;
  uint32_t _pad[4];
} __attribute__((aligned(16))) CameraUniform;

typedef struct {
  cclock *clock;
  float speed;
  CameraMode mode;
  float sensitivity;
  float wheel_sensitivity;
} CameraCreateDescriptor;

typedef struct {

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

Camera camera_create(const CameraCreateDescriptor *);
void camera_reset(Camera *);
void camera_set_mode(Camera *, CameraMode);
void camera_update_matrix_uniform(void *, void *);
void camera_draw(Camera *);
void camera_look_at(Camera *, vec3, vec3);

// get
float camera_position(const Camera *);
float camera_euler_rotation(const Camera *);
mat4 *camera_view(Camera *);
CameraUniform camera_uniform(Camera *);

// set
void camera_translate(Camera *, vec3);
void camera_rotate(Camera *, vec3);
void camera_update_view(Camera *);

#endif
