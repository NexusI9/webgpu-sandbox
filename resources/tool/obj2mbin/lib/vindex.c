#include "vindex.h"
#include "file.h"
#include <stdio.h>
#include <string.h>


int index_triangulate() { return 0; }

/**
   Wavefront OBJ structure:

   vNosition:          vec3[]
   vNormal:            vec3[]
   vTextureCoordinate: vec2[]
   index:              Vp / vT / vN... []

   Need to convert to our vertex format:
   vertex attribute: position(3) normal(3) color(3) uv(2)...
   vertex index: f32[]

 */
