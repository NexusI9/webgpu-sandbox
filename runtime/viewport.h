#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_

#include "../include/cglm/mat4.h"

typedef struct {
    float fov;
    float near_clip;
    float far_clip;
    mat4 projection;
} viewport;


viewport viewport_create(float fov, float near_clip, float far_clip);


#endif
