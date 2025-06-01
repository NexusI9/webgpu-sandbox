#ifndef _LIGHT_CREATE_H_
#define _LIGHT_CREATE_H_
#include "core.h"

// constructors
void light_create_point(PointLight *, PointLightDescriptor *);
void light_create_spot(SpotLight *, SpotLightDescriptor *);
void light_create_ambient(AmbientLight *, AmbientLightDescriptor *);
void light_create_sun(SunLight *, SunLightDescriptor *);


#endif
