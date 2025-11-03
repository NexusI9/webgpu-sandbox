#ifndef _THEME_DEFAULT_ICON_H_
#define _THEME_DEFAULT_ICON_H_

#include "../../core.h"

static const ivec2 theme_default_icon[] = {

    [ThemeIcon_Null] = {15, 15},
    // render modes
    [ThemeIcon_RenderMode_Boundbox] = {5, 0},
    [ThemeIcon_RenderMode_Wireframe] = {6, 0},
    [ThemeIcon_RenderMode_Solid] = {7, 0},
    [ThemeIcon_RenderMode_Texture] = {8, 0},

    // gizmo
    [ThemeIcon_Gizmo_Position] = {9, 0},
    [ThemeIcon_Gizmo_Rotate] = {10, 0},
    [ThemeIcon_Gizmo_Scale] = {11, 0},

    // log
    [ThemeIcon_Log_Error] = {12, 0},
    [ThemeIcon_Log_Warning] = {13, 0},
    [ThemeIcon_Log_Info] = {14, 0},
    [ThemeIcon_Log_Success] = {15, 0},
    [ThemeIcon_Log_Import] = {0, 1},
    [ThemeIcon_Log_Process] = {1, 1},

    // bool
    [ThemeIcon_Layout] = {2, 1},
    [ThemeIcon_Activity] = {3, 1},
    [ThemeIcon_Eye] = {4, 1},
    [ThemeIcon_EyeOff] = {5, 1},

    // prop tab
    [ThemeIcon_Properties_Scene] = {6, 1},
    [ThemeIcon_Properties_Setting] = {7, 1},
    [ThemeIcon_Properties_Object] = {8, 1},
    [ThemeIcon_Properties_Chip] = {13, 1},
    [ThemeIcon_Properties_Clock] = {14, 1},
    [ThemeIcon_Properties_Earth] = {15, 1},

    // solid
    [ThemeIcon_PointLight] = {0, 0},
    [ThemeIcon_SunLight] = {1, 0},
    [ThemeIcon_SpotLight] = {2, 0},
    [ThemeIcon_AmbientLight] = {3, 0},
    [ThemeIcon_Mesh] = {9, 1},
    [ThemeIcon_ProbeReflectionPlane] = {10, 1},
    [ThemeIcon_ProbeReflectionGrid] = {11, 1},
    [ThemeIcon_Camera] = {4, 0},
    [ThemeIcon_Grid] = {12, 1},
};

#endif
