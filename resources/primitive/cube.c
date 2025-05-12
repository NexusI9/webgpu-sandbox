#include "cube.h"

static float cube_vertex_data[] = {
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // Bottom-left
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // Bottom-right
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, // Top-right
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f, // Top-left

    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f, 1.0f,  0.0f, 0.0f, // Bottom-left
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f, 1.0f,  1.0f, 0.0f, // Bottom-right
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f, // Top-right
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f, // Top-left
};

static uint16_t cube_index_data[] = { // Front face
    0, 1, 2, 0, 2, 3,

    // Back face
    5, 4, 7, 5, 7, 6,

    // Left face
    4, 0, 3, 4, 3, 7,

    // Right face
    1, 5, 6, 1, 6, 2,

    // Top face
    3, 2, 6, 3, 6, 7,

    // Bottom face
    4, 5, 1, 4, 1, 0};

primitive primitive_cube() {

  return (primitive){
      // vertex data
      .vertex =
          {
              .entries = cube_vertex_data,
              .length = sizeof(cube_vertex_data) / sizeof(cube_vertex_data[0]),
          },

      // index data
      .index =
          {
              .entries = cube_index_data,
              .length = sizeof(cube_index_data) / sizeof(cube_index_data[0]),
          },
  };
}
