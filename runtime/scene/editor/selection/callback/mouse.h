#ifndef _SCENE_EDITOR_CALLBACK_MOUSE_H_
#define _SCENE_EDITOR_CALLBACK_MOUSE_H_

#include "../../../core.h"

void scene_selection_init_mouse_events(Scene* scene);

// camera raycast callbacks
void scene_selection_raycast_mesh_callback(CameraRaycastCallback *,
                                           const EmscriptenMouseEvent *,
                                           void *);


void scene_selection_raycast_gizmo_callback(CameraRaycastCallback *,
                                            const EmscriptenMouseEvent *,
                                            void *);


// mouse event callback
bool scene_selection_reset_callback(int, const EmscriptenMouseEvent *, void *);


#endif
