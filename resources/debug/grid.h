#ifndef _GRID_H_
#define _GRID_H_

#include <stdint.h>

typedef struct {

  uint16_t size;
  float cell_size;

} GridCreateDescriptor;

typedef struct {

  uint16_t size;
  float cell_size;

} grid;

grid grid_create(const GridCreateDescriptor *);

#endif
