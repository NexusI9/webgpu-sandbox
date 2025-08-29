#include "list.h"
#include "core.h"

StaticListStatus light_list_create(LightList *list, size_t capacity) {

  // init point light list
  stli_create(&list->point.base.capacity, &list->point.base.length, capacity,
              "Point Light List Base");

  stli_create(&list->point.shadow.capacity, &list->point.shadow.length,
              capacity, "Point Light List Shadow");

  // init spot light list
  stli_create(&list->spot.base.capacity, &list->spot.base.length, capacity,
              "Spot Light List Base");

  stli_create(&list->spot.shadow.capacity, &list->spot.shadow.length, capacity,
              "Spot Light List Shadow");

  // init sun light list
  stli_create(&list->sun.base.capacity, &list->sun.base.length, capacity,
              "Sun Light List Base");

  stli_create(&list->sun.shadow.capacity, &list->sun.shadow.length, capacity,
              "Sun Light List Shadow");

  // init ambient light list
  stli_create(&list->ambient.capacity, &list->ambient.length, capacity,
              "Ambient Light List Base");

  return StaticListStatus_Success;
}

StaticListStatus light_list_point_shadow_insert(PointLightListShadow *list,
                                                PointLight *light) {
  return stli_insert((void *)list->entries, &list->capacity, &list->length,
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
  return stli_insert((void *)list->entries, &list->capacity, &list->length,
                     sizeof(SunLight *), (void *)&light,
                     "Sun Light List Shadow");
}

StaticListStatus light_list_sun_shadow_remove(SunLightListShadow *list,
                                              SunLight *light) {
  return stli_remove((void *)list->entries, &list->length, sizeof(SunLight *),
                     (void *)&light, "Sun Light List Shadow");
}

StaticListStatus light_list_spot_shadow_insert(SpotLightListShadow *list,
                                               SpotLight *light) {
  return stli_insert((void *)list->entries, &list->capacity, &list->length,
                     sizeof(SpotLight *), (void *)&light,
                     "Spot Light List Shadow");
}

StaticListStatus light_list_spot_shadow_remove(SpotLightListShadow *list,
                                               SpotLight *light) {
  return stli_remove((void *)list->entries, &list->length, sizeof(SpotLight *),
                     (void *)&light, "Spot Light List Shadow");
}
