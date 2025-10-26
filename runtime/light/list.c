#include "list.h"

#include "backend/logger.h"
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

StaticListStatus light_list_point_shadow_insert(PointLightListShadow *list,
                                                PointLight *light) {
  return stli_insert((void *)list->entries, list->capacity, &list->length,
                     sizeof(PointLight *), (void *)&light,
                     "Point Light List Shadow");
}
StaticListStatus light_list_point_shadow_remove(PointLightListShadow *list,
                                                PointLight *light) {
  return stli_remove((void *)list->entries, &list->length, sizeof(PointLight *),
                     (void *)light, "Point Light List Shadow");
}

StaticListStatus light_list_sun_shadow_insert(SunLightListShadow *list,
                                              SunLight *light) {
  return stli_insert((void *)list->entries, list->capacity, &list->length,
                     sizeof(SunLight *), (void *)&light,
                     "Sun Light List Shadow");
}

StaticListStatus light_list_sun_shadow_remove(SunLightListShadow *list,
                                              SunLight *light) {
  return stli_remove((void *)list->entries, &list->length, sizeof(SunLight *),
                     (void *)light, "Sun Light List Shadow");
}

StaticListStatus light_list_spot_shadow_insert(SpotLightListShadow *list,
                                               SpotLight *light) {
  return stli_insert((void *)list->entries, list->capacity, &list->length,
                     sizeof(SpotLight *), (void *)&light,
                     "Spot Light List Shadow");
}

StaticListStatus light_list_spot_shadow_remove(SpotLightListShadow *list,
                                               SpotLight *light) {
  return stli_remove((void *)list->entries, &list->length, sizeof(SpotLight *),
                     (void *)light, "Spot Light List Shadow");
}

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

  logger_add(LoggerFlag_Error, "Couldn't create new light uniform slot. Light "
                               "list reached max capacity.");

  return (LightListSlot){.uniform = 0, .id = UBO_INDEX_UNFOUND, .offset = 0};
}
