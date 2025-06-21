#ifndef _CAMERA_MODE_H_
#define _CAMERA_MODE_H_

#include "core.h"

void camera_set_mode(Camera *, CameraMode);

void camera_mode_draw(Camera *);
void camera_mode_flying_controller(Camera *);
void camera_mode_orbit_controller(Camera *);
void camera_mode_edit_controller(Camera *);

#endif
