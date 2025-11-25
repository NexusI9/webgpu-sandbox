#ifndef _THEME_H_
#define _THEME_H_

#include "backend/logger.h"
#include "runtime/texture/sprite_sheet.h"
#include "utils/color.h"
#include "utils/defines.h"
#include "webgpu/webgpu.h"

typedef enum {
  ThemeStatus_Success,
  ThemeStatus_MissingResource,
  ThemeStatus_UndefError,
} ThemeStatus;

#define THEME_COLOR_COUNT 145
typedef enum {
  ThemeColor_Surface_Highest,
  ThemeColor_Surface_Higher,
  ThemeColor_Surface_High,
  ThemeColor_Surface_Base,
  ThemeColor_Surface_Low,
  ThemeColor_Surface_Lower,
  ThemeColor_Surface_Lowest,
  ThemeColor_Background_Blanket_Light,
  ThemeColor_Background_Blanket_Medium,
  ThemeColor_Background_Blanket_Strong,
  ThemeColor_Background_System_Base_On_Light,
  ThemeColor_Background_System_Base_On_Dark,
  ThemeColor_Background_System_Subtle_On_Light,
  ThemeColor_Background_System_Subtle_On_Dark,
  ThemeColor_Text_Subtle_On_Dark,
  ThemeColor_Text_Subtle_On_Light,
  ThemeColor_Icon_Subtle_On_Light,
  ThemeColor_Icon_Subtle_On_Dark,
  ThemeColor_Border_Subtle_On_Light,
  ThemeColor_Border_Subtle_On_Dark,
  ThemeColor_Background_System_Strong_On_Light,
  ThemeColor_Background_System_Strong_On_Dark,
  ThemeColor_Background_System_Hovered_On_Light,
  ThemeColor_Background_System_Hovered_On_Dark,
  ThemeColor_Background_Brand_Base,
  ThemeColor_Background_Brand_Subtle,
  ThemeColor_Background_Brand_Strong,
  ThemeColor_Background_Brand_Hovered,
  ThemeColor_Text_Brand_Base,
  ThemeColor_Text_Brand_Subtle,
  ThemeColor_Text_Brand_Strong,
  ThemeColor_Text_Brand_On_Brand,
  ThemeColor_Icon_Brand_Base,
  ThemeColor_Icon_Brand_Subtle,
  ThemeColor_Icon_Brand_Strong,
  ThemeColor_Icon_Brand_On_Brand,
  ThemeColor_Border_Brand_On_Brand,
  ThemeColor_Border_Brand_Subtle,
  ThemeColor_Border_Brand_Base,
  ThemeColor_Border_Brand_Strong,
  ThemeColor_Background_Secondary_Base,
  ThemeColor_Background_Secondary_Subtle,
  ThemeColor_Background_Secondary_Strong,
  ThemeColor_Background_Secondary_Hovered,
  ThemeColor_Text_Secondary_Base,
  ThemeColor_Text_Secondary_Subtle,
  ThemeColor_Text_Secondary_Strong,
  ThemeColor_Text_Secondary_On_Secondary,
  ThemeColor_Icon_Secondary_Base,
  ThemeColor_Icon_Secondary_Subtle,
  ThemeColor_Icon_Secondary_Strong,
  ThemeColor_Icon_Secondary_On_Secondary,
  ThemeColor_Border_Secondary_On_Secondary,
  ThemeColor_Border_Secondary_Subtle,
  ThemeColor_Border_Secondary_Base,
  ThemeColor_Border_Secondary_Strong,
  ThemeColor_Background_Danger_Base,
  ThemeColor_Background_Danger_Subtle,
  ThemeColor_Background_Danger_Strong,
  ThemeColor_Background_Danger_Strong_Dark,
  ThemeColor_Background_Danger_Hovered,
  ThemeColor_Text_Danger_Base,
  ThemeColor_Text_Danger_Subtle,
  ThemeColor_Text_Danger_Strong,
  ThemeColor_Text_Danger_On_Danger,
  ThemeColor_Icon_Danger_Base,
  ThemeColor_Icon_Danger_Subtle,
  ThemeColor_Icon_Danger_Strong,
  ThemeColor_Icon_Danger_On_Danger,
  ThemeColor_Border_Danger_On_Danger,
  ThemeColor_Border_Danger_Subtle,
  ThemeColor_Border_Danger_Base,
  ThemeColor_Border_Danger_Strong,
  ThemeColor_Background_Warning_Base,
  ThemeColor_Background_Warning_Subtle,
  ThemeColor_Background_Warning_Strong,
  ThemeColor_Background_Warning_Strong_Dark,
  ThemeColor_Background_Warning_Hovered,
  ThemeColor_Text_Warning_Base,
  ThemeColor_Text_Warning_Subtle,
  ThemeColor_Text_Warning_Strong,
  ThemeColor_Text_Warning_On_Warning,
  ThemeColor_Icon_Warning_Base,
  ThemeColor_Icon_Warning_Subtle,
  ThemeColor_Icon_Warning_Strong,
  ThemeColor_Icon_Warning_On_Warning,
  ThemeColor_Border_Warning_On_Warning,
  ThemeColor_Border_Warning_Subtle,
  ThemeColor_Border_Warning_Base,
  ThemeColor_Border_Warning_Strong,
  ThemeColor_Background_Success_Base,
  ThemeColor_Background_Success_Subtle,
  ThemeColor_Background_Success_Strong,
  ThemeColor_Background_Success_Strong_Dark,
  ThemeColor_Background_Success_Hovered,
  ThemeColor_Text_Success_Base,
  ThemeColor_Text_Success_Subtle,
  ThemeColor_Text_Success_Strong,
  ThemeColor_Text_Success_On_Success,
  ThemeColor_Icon_Success_Base,
  ThemeColor_Icon_Success_Subtle,
  ThemeColor_Icon_Success_Strong,
  ThemeColor_Icon_Success_On_Success,
  ThemeColor_Border_Success_On_Success,
  ThemeColor_Border_Success_Subtle,
  ThemeColor_Border_Success_Base,
  ThemeColor_Border_Success_Strong,
  ThemeColor_Background_Information_Base,
  ThemeColor_Background_Information_Subtle,
  ThemeColor_Background_Information_Strong,
  ThemeColor_Background_Information_Strong_Dark,
  ThemeColor_Background_Information_Hovered,
  ThemeColor_Text_Information_Base,
  ThemeColor_Text_Information_Subtle,
  ThemeColor_Text_Information_Strong,
  ThemeColor_Text_Information_On_Information,
  ThemeColor_Icon_Information_Base,
  ThemeColor_Icon_Information_Subtle,
  ThemeColor_Icon_Information_Strong,
  ThemeColor_Icon_Information_On_Information,
  ThemeColor_Border_Information_On_Information,
  ThemeColor_Border_Information_Subtle,
  ThemeColor_Border_Information_Base,
  ThemeColor_Border_Information_Strong,
  ThemeColor_Background_Disabled_On_Light,
  ThemeColor_Background_Disabled_On_Dark,
  ThemeColor_Text_Disabled_On_Light,
  ThemeColor_Text_Disabled_On_Dark,
  ThemeColor_Icon_Disabled_On_Light,
  ThemeColor_Icon_Disabled_On_Dark,
  ThemeColor_Border_Disabled_On_Light,
  ThemeColor_Border_Disabled_On_Dark,
  ThemeColor_Text_On_Dark,
  ThemeColor_Text_On_Light,
  ThemeColor_Text_Interactive,
  ThemeColor_Text_Subtlest_On_Dark,
  ThemeColor_Text_Subtlest_On_Light,
  ThemeColor_Icon_Subtlest_On_Light,
  ThemeColor_Icon_Subtlest_On_Dark,
  ThemeColor_Border_Subtlest_On_Light,
  ThemeColor_Border_Subtlest_On_Dark,
  ThemeColor_Icon_On_Dark,
  ThemeColor_Icon_On_Light,
  ThemeColor_Border_On_Dark,
  ThemeColor_Border_On_Light,
} ThemeColor;

#define THEME_SIZE_COUNT 21
typedef enum {
  ThemeSize_RightPanel_Width,
  ThemeSize_RightPanelTab_Width,
  ThemeSize_Tree_Height,
  ThemeSize_Tree_PaddingV,
  ThemeSize_Tree_PaddingH,
  ThemeSize_TopBar_Height,
  ThemeSize_TopBar_Margin,
  ThemeSize_Gizmo_Width,
  ThemeSize_Gizmo_Height,
  ThemeSize_Gizmo_Margin,
  ThemeSize_Button_RenderModeSize,
  ThemeSize_Button_GizmoSize,
  ThemeSize_Button_InspectorTab,
  ThemeSize_Button_DisplaySize,
  ThemeSize_BottomPanel_Height,
  ThemeSize_Log_IconScale,
  ThemeSize_Monitor_Width,
  ThemeSize_Monitor_Height,
  ThemeSize_Space_Small,
  ThemeSize_Space_Medium,
  ThemeSize_Space_Large,
} ThemeSize;

#define THEME_ICON_COUNT 33
typedef enum {
  ThemeIcon_Null,
  ThemeIcon_RenderMode_Boundbox,
  ThemeIcon_RenderMode_Wireframe,
  ThemeIcon_RenderMode_Solid,
  ThemeIcon_RenderMode_Texture,
  ThemeIcon_Gizmo_Position,
  ThemeIcon_Gizmo_Rotate,
  ThemeIcon_Gizmo_Scale,
  ThemeIcon_Log_Error,
  ThemeIcon_Log_Warning,
  ThemeIcon_Log_Info,
  ThemeIcon_Log_Success,
  ThemeIcon_Log_Import,
  ThemeIcon_Log_Process,
  ThemeIcon_Layout,
  ThemeIcon_Activity,
  ThemeIcon_Eye,
  ThemeIcon_EyeOff,
  ThemeIcon_Properties_Scene,
  ThemeIcon_Properties_Setting,
  ThemeIcon_Properties_Object,
  ThemeIcon_Properties_Chip,
  ThemeIcon_Properties_Clock,
  ThemeIcon_Properties_Earth,
  ThemeIcon_PointLight,
  ThemeIcon_AmbientLight,
  ThemeIcon_SunLight,
  ThemeIcon_SpotLight,
  ThemeIcon_Mesh,
  ThemeIcon_ProbeReflectionPlane,
  ThemeIcon_ProbeReflectionGrid,
  ThemeIcon_Camera,
  ThemeIcon_Grid,
} ThemeIcon;

typedef struct {
  ivec2 cell;
  vec2 uv0;
  vec2 uv1;
} ThemeIconCell;

typedef struct {
  const char *label;

  const color *colors;
  const int *sizes;

  ThemeIconCell icons[THEME_ICON_COUNT];

  double dpi;
  TextureSpriteSheet sprite_sheet_texture;

} Theme;

typedef struct {
  const char *label;
  const color *colors;
  const int *sizes;
  double dpi;
} ThemeDescriptor;

extern Theme g_theme;

EXTERN_C_BEGIN

ThemeStatus theme_init(Theme *, const ThemeDescriptor *);

static inline ThemeStatus
theme_create_icon_atlas(Theme *theme,
                        const TextureSpriteSheetDescriptor *desc) {
  texture_sprite_sheet_create(&theme->sprite_sheet_texture, desc);
  return ThemeStatus_Success;
}

static inline ThemeStatus theme_set_icons_coordinates(Theme *theme,
                                                      const ivec2 *coo) {

  if (theme->sprite_sheet_texture.cell_size[0] == 0 ||
      theme->sprite_sheet_texture.cell_size[0] == 0) {
    logger_add(LoggerFlag_Error,
               "Current theme atlas texture has a cell size of 0, make sure "
               "the texture atlas is initialized correctly.");
    return ThemeStatus_MissingResource;
  }

  for (uint8_t i = 0; i < THEME_ICON_COUNT; i++) {
    glm_ivec2_copy((int *)coo[i], theme->icons[i].cell);
    texture_sprite_sheet_cell_uv(&theme->sprite_sheet_texture,
                                 theme->icons[i].cell, theme->icons[i].uv0,
                                 theme->icons[i].uv1);
  }

  return ThemeStatus_Success;
}

static inline const int theme_size(const Theme *theme, const ThemeSize size) {
  return theme->sizes[size] * theme->dpi;
}

static inline const float *theme_color(const Theme *theme,
                                       const ThemeColor color) {
  return theme->colors[color];
}

static inline const ThemeIconCell *theme_icon_cell(const Theme *theme,
                                                   const ThemeIcon icon) {
  return &theme->icons[icon];
}

static inline WGPUTextureView theme_icon_atlas(const Theme *theme) {
  return theme->sprite_sheet_texture.view;
}

static inline int theme_scale_size(const Theme *theme, const int size) {
  return size * theme->dpi;
}

EXTERN_C_END

#endif
