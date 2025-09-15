#ifndef _CAMERA_MODE_H_
#define _CAMERA_MODE_H_

#include "core.h"

typedef void (*camera_mode_controller_callback)(Camera *);

void camera_set_mode(Camera *, CameraMode);
void camera_mode_flying_controller(Camera *);
void camera_mode_orbit_controller(Camera *);
void camera_mode_edit_controller(Camera *);

static const camera_mode_controller_callback
    camera_mode_controller[CAMERA_MODE_COUNT] = {
        [CameraMode_Edit] = camera_mode_edit_controller,
        [CameraMode_Flying] = camera_mode_flying_controller,
        [CameraMode_Orbit] = camera_mode_orbit_controller,
};

#endif
