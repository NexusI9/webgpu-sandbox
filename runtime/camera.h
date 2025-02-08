#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "../include/cglm/mat4.h"
#include "../include/cglm/vec3.h"

typedef struct {
  mat4 view;
  vec3 position;
} CameraUniform;

typedef struct {
  vec3 position;
  vec3 euler_rotation;
  mat4 view;
} camera;

camera camera_create();
void camera_reset(camera *);

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
