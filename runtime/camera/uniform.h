#ifndef _CAMERA_UNIFORM_H_
#define _CAMERA_UNIFORM_H_

#include "core.h"

CameraUniform *camera_uniform(Camera *);
void camera_uniform_update(Camera *);
void camera_uniform_update_matrix_callback(void *, void *);
bool camera_uniform_compare_views_callback(void *, const void *);

#endif
