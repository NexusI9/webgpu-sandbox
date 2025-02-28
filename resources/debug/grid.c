#include "grid.h"

grid grid_create(const GridCreateDescriptor *gd) {

  grid new_grid;
  new_grid.cell_size = gd->cell_size;
  new_grid.size = gd->size;

  return new_grid;
}


