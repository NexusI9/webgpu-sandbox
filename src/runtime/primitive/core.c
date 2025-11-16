#include "core.h"

void primitive_destroy(Primitive *primitive) {
  vertex_attribute_destroy(&primitive->vertex);
  vertex_index_destroy(&primitive->index);
}
