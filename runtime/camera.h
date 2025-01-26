#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "../include/cglm/mat4.h"
#include "../include/cglm/vec3.h"

typedef struct{
    vec3 position;
    vec3 euler_rotation;
    mat4 view_matrix;
} camera;


camera camera_create();
void camera_reset(camera *);
float*  camera_position_get(const camera *);
float* camera_euler_rotation_get(const camera *);
float* camera_view_matrix_get(const camera *);

#endif
