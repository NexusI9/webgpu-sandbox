#ifndef _LIGHT_LIST_H_
#define _LIGHT_LIST_H_
#include "core.h"

#define LIGHT_LIST_SUCCESS 0
#define LIGHT_LIST_ALLOC_FAIL 1
#define LIGHT_LIST_ERROR 2

typedef struct {
  PointLightList point;
  SpotLightList spot;
  AmbientLightList ambient;
  SunLightList sun;
} LightList;


int light_list_create(LightList*, size_t);


#endif
