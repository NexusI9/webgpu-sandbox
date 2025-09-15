#include "core.h"

#include <stdlib.h>

void texture_save(Texture *texture, const char *path) {}

void texture_free(Texture *texture) { free(texture->data); }
