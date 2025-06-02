#ifndef _TEXTURE_CREATE_H_
#define _TEXTURE_CREATE_H_

#include "core.h"

void texture_create(Texture *, const TextureCreateDescriptor *);
int texture_create_from_file(Texture *, const char *);
void texture_create_from_ref(unsigned char **, size_t *,
                           const TextureCreateDescriptor *);


#endif
