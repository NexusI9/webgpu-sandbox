#include "list.h"

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
