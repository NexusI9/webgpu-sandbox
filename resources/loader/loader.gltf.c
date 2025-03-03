#define CGLTF_IMPLEMENTATION
#include "loader.gltf.h"
#include <stdio.h>

static void loader_gltf_create_mesh(mesh *, cgltf_data *);
static void loader_gltf_create_shader(shader *, cgltf_data *);

void loader_gltf_load(mesh *mesh, const char *path,
                      const cgltf_options *options) {

  cgltf_data *data = NULL;
  cgltf_result result = cgltf_parse_file(options, path, &data);

  switch (result) {

  case cgltf_result_invalid_json:
    perror("Invalid GLTF JSON\n"), exit(1);
    return;

  case cgltf_result_success:
    printf("successfully imported GLTF\n");
    cgltf_free(data);
    return;

  case cgltf_result_file_not_found:
    perror("GLTF file not found\n"), exit(1);
    return;

  case cgltf_result_out_of_memory:
    perror("GLTF loading aborted, out of memory\n"), exit(1);
    return;

  default:
    perror("GLTF loading aborted, unhanded error\n"), exit(1);
    return;
  }
}
