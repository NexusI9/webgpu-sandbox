#include "plane.h"

static float plane_vertex_data[] = {
    // Positions          // Normals            // Colors          // UVs
    -0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Bottom-left
     0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom-right
     0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,  1.0f, 1.0f, // Top-right
    -0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, 0.0f,  0.0f, 1.0f  // Top-left
};

static uint16_t plane_index_data[] = {
    0, 1, 2,  // First triangle (Bottom-left → Bottom-right → Top-right)
    2, 3, 0   // Second triangle (Top-right → Top-left → Bottom-left)
};

primitive primitive_plane() {

  return (primitive){
      // vertex data
      .vertex =
          {
              .entries = plane_vertex_data,
              .length =
                  sizeof(plane_vertex_data) / sizeof(plane_vertex_data[0]),
          },

      // index data
      .index =
          {
              .entries = plane_index_data,
              .length = sizeof(plane_index_data) / sizeof(plane_index_data[0]),
          },
  };
}
