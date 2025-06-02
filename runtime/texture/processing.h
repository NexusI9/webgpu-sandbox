#ifndef _TEXTURE_PROCESSING_H_
#define _TEXTURE_PROCESSING_H_

#include "core.h"

void texture_fill(Texture *, int);
void texture_blur(const Texture *, int, float, texture_data *);
void texture_contrast(const Texture *, float, texture_data *);
void texture_remap(const Texture *, int, int, texture_data *);


#endif
