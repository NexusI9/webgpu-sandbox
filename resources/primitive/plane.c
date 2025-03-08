#include "plane.h"

static float plane_vertex_data[] = {
    // Positions          // Normals            // Colors          // UVs
    -0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Bottom-left
     0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom-right
     0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,  1.0f, 1.0f, // Top-right
    -0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, 0.0f,  0.0f, 1.0f  // Top-left
};

static uint16_t plane_index_data[] = {
    0, 1, 2, // First triangle
    0, 2, 3  // Second triangle
};

primitive primitive_plane() {

  return (primitive){
      // vertex data
      .vertex =
          {
              .data = plane_vertex_data,
              .length =
                  sizeof(plane_vertex_data) / sizeof(plane_vertex_data[0]),
          },

      // index data
      .index =
          {
              .data = plane_index_data,
              .length = sizeof(plane_index_data) / sizeof(plane_index_data[0]),
          },
  };
}
