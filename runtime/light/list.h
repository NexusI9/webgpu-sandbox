#ifndef _LIGHT_LIST_H_
#define _LIGHT_LIST_H_
#include "core.h"


typedef enum{
  LightListStatus_Success,
  LightListStatus_AlloFail,
  LightListStatus_Error,
} LightListStatus;

typedef struct {
  PointLightList point;
  SpotLightList spot;
  AmbientLightList ambient;
  SunLightList sun;
} LightList;


LightListStatus light_list_create(LightList*, size_t);


#endif
