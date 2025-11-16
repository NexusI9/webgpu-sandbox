#include "list.h"

#include "backend/logger.h"
#include "backend/renderer/shadow_map/core.h"
#include "backend/resource_manager.h"
#include "backend/ubo.h"
#include "core.h"
#include "utils/stli.h"

StaticListStatus light_list_create(LightList *list, size_t capacity) {

  const struct {
    size_t *capacity;
    size_t *length;
    const char *label;
  } light_static_list[7] = {
      {
          .capacity = &list->point.base.capacity,
          .length = &list->point.base.length,
          .label = "Point Light List Base",
      },
      {
          .capacity = &list->point.shadow.capacity,
          .length = &list->point.shadow.length,
          .label = "Point Light List Shadow",
      },
      {
          .capacity = &list->spot.base.capacity,
          .length = &list->spot.base.length,
          .label = "Spot Light List Base",
      },
      {
          .capacity = &list->spot.shadow.capacity,
          .length = &list->spot.shadow.length,
          .label = "Spot Light List Shadow",
      },
      {
          .capacity = &list->sun.base.capacity,
          .length = &list->sun.base.length,
          .label = "Sun Light List Base",
      },
      {
          .capacity = &list->sun.shadow.capacity,
          .length = &list->sun.shadow.length,
          .label = "Sun Light List Shadow",
      },
      {
          .capacity = &list->ambient.capacity,
          .length = &list->ambient.length,
          .label = "Ambient Light List Base",
      },
  };

  for (size_t i = 0; i < 7; i++)
    stli_create(light_static_list[i].capacity, light_static_list[i].length,
                capacity, light_static_list[i].label);


  return StaticListStatus_Success;
}

#define LIGHT_LIST_NEW(Type, FuncName, List, Label)                            \
  Type *light_list_new_##FuncName(List *list) {                                \
                                                                               \
    Type *light = rem_new_##FuncName();                                        \
                                                                               \
    if (light == NULL)                                                         \
      return NULL;                                                             \
                                                                               \
    if (stli_insert((void *)list->entries, list->capacity, &list->length,      \
                    sizeof(Type *), (void *)&light,                            \
                    Label) != StaticListStatus_Success)                        \
      return NULL;                                                             \
                                                                               \
    return light;                                                              \
  }

LIGHT_LIST_NEW(PointLight, point_light, PointLightListBase, "Point Light");
LIGHT_LIST_NEW(AmbientLight, ambient_light, AmbientLightList, "Ambient Light");
LIGHT_LIST_NEW(SpotLight, spot_light, SpotLightListBase, "Spot Light");
LIGHT_LIST_NEW(SunLight, sun_light, SunLightListBase, "Sun Light");

#define LIGHT_LIST_SHADOW_INSERT(List, FuncName, Type, Label)                  \
  StaticListStatus light_list_##FuncName##_shadow_insert(List *list,           \
                                                         Type *light) {        \
                                                                               \
    return stli_insert((void *)list->entries, list->capacity, &list->length,   \
                       sizeof(Type *), (void *)&light, Label);                 \
  }

#define LIGHT_LIST_SHADOW_REMOVE(List, FuncName, Type, Label)                  \
  StaticListStatus light_list_##FuncName##_shadow_remove(List *list,           \
                                                         Type *light) {        \
    return stli_remove((void *)list->entries, &list->length, sizeof(Type *),   \
                       (void *)light, Label);                                  \
  }

LIGHT_LIST_SHADOW_INSERT(PointLightListShadow, point, PointLight,
                         "Point Light Shadow List");
LIGHT_LIST_SHADOW_REMOVE(PointLightListShadow, point, PointLight,
                         "Point Light Shadow List");

LIGHT_LIST_SHADOW_INSERT(SpotLightListShadow, spot, SpotLight,
                         "Spot Light Shadow List");
LIGHT_LIST_SHADOW_REMOVE(SpotLightListShadow, spot, SpotLight,
                         "Spot Light Shadow List");

LIGHT_LIST_SHADOW_INSERT(SunLightListShadow, sun, SunLight,
                         "Sun Light Shadow List");
LIGHT_LIST_SHADOW_REMOVE(SunLightListShadow, sun, SunLight,
                         "Sun Light Shadow List");

LightListSlot light_list_uniform_new_entry(LightListUniform *list,
                                           const LightType type) {

  switch (type) {

  case LightType_Ambient:

    if (list->ambient_count < LIGHT_LIST_ENTRIES_CAPACITY)
      return (LightListSlot){
          .uniform = {.ambient = &list->ambient_light[list->ambient_count]},
          .offset =
              light_list_uniform_offset(list->ambient_count, LightType_Ambient),
          .id = list->ambient_count++,
      };
    break;

  case LightType_Point:
    if (list->point_count < LIGHT_LIST_ENTRIES_CAPACITY)
      return (LightListSlot){
          .uniform = {.point = &list->point_light[list->point_count]},
          .offset =
              light_list_uniform_offset(list->point_count, LightType_Point),
          .id = list->point_count++,
      };
    break;

  case LightType_Spot:
    if (list->spot_count < LIGHT_LIST_ENTRIES_CAPACITY)
      return (LightListSlot){
          .uniform = {.spot = &list->spot_light[list->spot_count]},
          .offset = light_list_uniform_offset(list->spot_count, LightType_Spot),
          .id = list->spot_count++,
      };
    break;

  case LightType_Sun:
    if (list->sun_count < LIGHT_LIST_ENTRIES_CAPACITY)
      return (LightListSlot){
          .uniform = {.sun = &list->sun_light[list->sun_count]},
          .offset = light_list_uniform_offset(list->sun_count, LightType_Sun),
          .id = list->sun_count++,
      };
    break;
  }

  logger_add(LoggerFlag_Error, "Couldn't create new "
                               "light uniform slot. "
                               "Light "
                               "list reached max "
                               "capacity.");

  return (LightListSlot){.uniform = 0, .id = UBO_INDEX_UNFOUND, .offset = 0};
}
