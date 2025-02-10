#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "../include/cglm/mat4.h"
#include "../include/cglm/vec3.h"
#include "../include/cglm/vec4.h"

typedef struct {
  mat4 view;
  vec4 position;
} CameraUniform;

typedef enum { FIXED, FLYING, ORBIT } CameraMode;

typedef struct {
  vec3 position;
  vec3 euler_rotation;
  mat4 view;
  float speed;
} camera;

camera camera_create();
void camera_reset(camera *);
void camera_set_mode(camera *, CameraMode);
void camera_update_uniform(void *, void *);

// get
float camera_position(const camera *);
float camera_euler_rotation(const camera *);
mat4 *camera_view(const camera *);
CameraUniform camera_uniform(camera *);

// set
void camera_translate(camera *, vec3);
void camera_rotate(camera *, vec3);
void camera_update_view(camera *);

#endif
