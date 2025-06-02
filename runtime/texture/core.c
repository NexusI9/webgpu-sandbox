#include "core.h"
#include "../include/stb/stb_image.h"
#include "../utils/math.h"
#include "../utils/system.h"
#include "../utils/vector.h"
#include <stdlib.h>
#include <string.h>


void texture_save(Texture *texture, const char *path) {}

void texture_free(Texture *texture) { free(texture->data); }

