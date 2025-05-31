#include "gizmo.h"
#include "../resources/primitive/plane.h"
#include "../runtime/material.h"
#include "../runtime/texture.h"
#include "light.h"
#include "string.h"

static void *gizmo_list_expand(void **, size_t, size_t, size_t *);
static void *gizmo_list_alloc(void **, size_t, size_t, size_t *, size_t *);
static int gizmo_list_insert(const GizmoListInsertDescriptor *);
static void *gizmo_list_new(const GizmoListNewDescriptor *);
static int gizmo_mesh_list_copy(const GizmoMeshList *, GizmoMeshList *);

/**
   Create a plane mesh with a billboard shader
 */
void gizmo_create_billboard(Gizmo *mesh,
                            const GizmoCreateBillboardDescriptor *desc) {

  // create plane
  Primitive plane = primitive_plane();

  mesh_create_primitive(mesh, &(MeshCreatePrimitiveDescriptor){
                                  .primitive = plane,
                                  .device = desc->device,
                                  .queue = desc->queue,
                                  .name = "light",
                              });

  // set mesh position to light position
  mesh_position(mesh, *desc->position);

  // scale down gizmo
  mesh_scale(mesh, *desc->scale);

  // assign billboard shader
  mesh_set_shader(mesh, &(ShaderCreateDescriptor){
                            .device = desc->device,
                            .queue = desc->queue,
                            .label = "shader",
                            .name = "shader",
                            .path = SHADER_PATH_BILLBOARD,
                        });

  // set double side rendering
  pipeline_set_primitive(shader_pipeline(mesh_shader_texture(mesh)),
                         (WGPUPrimitiveState){
                             .frontFace = WGPUFrontFace_CCW,
                             .cullMode = WGPUCullMode_None,
                             .topology = WGPUPrimitiveTopology_TriangleList,
                             .stripIndexFormat = WGPUIndexFormat_Undefined,
                         });

  // bind view matrices
  material_texture_bind_views(mesh, desc->camera, desc->viewport, 0);

  // TODO: create UI Atlas
  Texture light_texture;
  texture_create_from_file(&light_texture, desc->texture_path);

  // bind texture + sampler
  material_texture_add_texture(
      mesh, &(ShaderCreateTextureDescriptor){
                .group_index = 1,
                .entry_count = 1,
                .visibility = WGPUShaderStage_Fragment,
                .entries = (ShaderBindGroupTextureEntry[]){{
                    .binding = 0,
                    .width = light_texture.width,
                    .height = light_texture.height,
                    .data = light_texture.data,
                    .size = light_texture.size,
                    .channels = light_texture.channels,
                    .dimension = WGPUTextureViewDimension_2D,
                    .format = WGPUTextureFormat_RGBA8Unorm,
                    .sample_type = WGPUTextureSampleType_Float,
                }},
            });

  material_texture_add_sampler(mesh,
                               &(ShaderCreateSamplerDescriptor){
                                   .group_index = 1,
                                   .entry_count = 1,
                                   .visibility = WGPUShaderStage_Fragment,
                                   .entries = (ShaderBindGroupSamplerEntry[]){{
                                       .binding = 1,
                                       .addressModeU = WGPUAddressMode_Repeat,
                                       .addressModeV = WGPUAddressMode_Repeat,
                                       .addressModeW = WGPUAddressMode_Repeat,
                                       .minFilter = WGPUFilterMode_Linear,
                                       .magFilter = WGPUFilterMode_Linear,
                                       .type = WGPUSamplerBindingType_Filtering,
                                       .compare = WGPUCompareFunction_Undefined,
                                   }},
                               });
}

void *gizmo_list_expand(void **dest, size_t capacity, size_t type_size,
                        size_t *new_capacity) {

  void *temp = realloc(dest, capacity * type_size);

  if (temp == NULL) {
    perror("Couldn't expand gizmo list\n");
    return temp;
  }

  *dest = temp;
  *new_capacity = capacity;

  return dest;
}

void *gizmo_list_alloc(void **dest, size_t capacity, size_t type_size,
                       size_t *new_capacity, size_t *new_length) {

  *dest = malloc(capacity * type_size);

  if (*dest == NULL) {
    perror("Couldn't allocate memory for gizmo list\n");
    *new_capacity = 0;
    *new_length = 0;
    return *dest;
  }

  *new_capacity = capacity;
  *new_length = 0;

  return *dest;
}

/**
   Dynamically allocate and initialize the gizmo lists (point, ambient,
   camera...)
 */
int gizmo_list_create(GizmoList *list, size_t capacity) {

  // create point light
  gizmo_list_alloc((void **)&list->point_light.entries, capacity,
                   sizeof(GizmoPointLight), &list->point_light.capacity,
                   &list->point_light.length);

  // create sun light
  gizmo_list_alloc((void **)&list->sun_light.entries, capacity,
                   sizeof(GizmoSunLight), &list->sun_light.capacity,
                   &list->sun_light.length);

  // create ambient light
  gizmo_list_alloc((void **)&list->ambient_light.entries, capacity,
                   sizeof(GizmoAmbientLight), &list->ambient_light.capacity,
                   &list->ambient_light.length);

  // create spot light
  gizmo_list_alloc((void **)&list->spot_light.entries, capacity,
                   sizeof(GizmoSpotLight), &list->spot_light.capacity,
                   &list->spot_light.length);

  // create camera
  gizmo_list_alloc((void **)&list->camera.entries, capacity,
                   sizeof(GizmoCamera), &list->camera.capacity,
                   &list->camera.length);

  return GIZMO_SUCCESS;
}

/**
   Function that abstracts the gizmo list insertion process.
   Inserts a given item in the list entries depending on its type size.
 */
int gizmo_list_insert(const GizmoListInsertDescriptor *desc) {

  // check if list is init
  if (desc->entries == NULL) {
    perror("Gizmo list not initialized\n");
    return GIZMO_ERROR;
  }

  size_t new_capacity = 2 * (*desc->capacity);
  // check list capacity
  if (*desc->length == *desc->capacity &&
      gizmo_list_expand(desc->entries, new_capacity, sizeof(desc->type_size),
                        desc->capacity) != GIZMO_SUCCESS)
    return GIZMO_ERROR;

  // copy item to list memory
  memcpy(&desc->entries[(*desc->length)++], desc->new_entry,
         sizeof(desc->type_size));

  return GIZMO_SUCCESS;
}

/**
   Function that abstracts the gizmo list get new item process.
   Return the pointer to the newly "created" element
 */
void *gizmo_list_new(const GizmoListNewDescriptor *desc) {

  // check if list is init
  if (desc->entries == NULL) {
    perror("Gizmo list not initialized\n");
    return NULL;
  }

  size_t new_capacity = 2 * (*desc->capacity);
  // check list capacity
  if (*desc->length == *desc->capacity &&
      gizmo_list_expand(desc->entries, new_capacity, sizeof(desc->type_size),
                        desc->capacity) != GIZMO_SUCCESS)
    return NULL;

  return desc->entries[(*desc->length)++];
}

/**
   Copy a Gizmo Mesh list from a source to a given desination
 */
int gizmo_mesh_list_copy(const GizmoMeshList *src, GizmoMeshList *dest) {

  // copy length
  dest->length = src->length;
  dest->entries = malloc(dest->length * sizeof(Mesh *));

  if (dest->entries) {
    perror("Couldn't allocate memory for gizmo mesh list\n");
    dest->length = 0;
    return GIZMO_ALLOC_FAIL;
  }

  // copy meshes pointer
  memcpy(dest->entries, src->entries, dest->length * sizeof(Mesh *));
  return GIZMO_SUCCESS;
}

/**
   Insert given light to the point light gizmo
 */
int gizmo_list_insert_point_light(GizmoList *list, PointLight *light,
                                  GizmoMeshList *mesh_list) {

  // init new entry
  GizmoPointLight new_entry = {light};

  // copy mesh list
  if (gizmo_mesh_list_copy(mesh_list, &new_entry.meshes) != GIZMO_SUCCESS)
    return GIZMO_ALLOC_FAIL;

  // insert new entry (use abstract function gizmo_list_insert)
  return gizmo_list_insert(&(GizmoListInsertDescriptor){
      .type_size = sizeof(GizmoPointLight),
      .new_entry = &new_entry,
      .capacity = &list->point_light.capacity,
      .length = &list->point_light.length,
      .entries = (void **)&list->point_light.entries,
  });
}
PointLight *gizmo_list_new_point_light(GizmoList *list) {
  return (PointLight *)gizmo_list_new(&(GizmoListNewDescriptor){
      .type_size = sizeof(GizmoPointLight),
      .capacity = &list->point_light.capacity,
      .length = &list->point_light.length,
      .entries = (void **)&list->point_light.entries,
  });
}

// ambient light gizmo
int gizmo_list_insert_ambient_light(GizmoList *list, AmbientLight *light,
                                    GizmoMeshList *mesh_list) {

  // init new entry
  GizmoAmbientLight new_entry = {light};

  // copy mesh list
  if (gizmo_mesh_list_copy(mesh_list, &new_entry.meshes) != GIZMO_SUCCESS)
    return GIZMO_ALLOC_FAIL;

  // insert new entry (use abstract function gizmo_list_insert)
  return gizmo_list_insert(&(GizmoListInsertDescriptor){
      .type_size = sizeof(GizmoAmbientLight),
      .new_entry = &new_entry,
      .capacity = &list->ambient_light.capacity,
      .length = &list->ambient_light.length,
      .entries = (void **)&list->ambient_light.entries,
  });
}
AmbientLight *gizmo_list_new_ambient_light(GizmoList *list) {
  return (AmbientLight *)gizmo_list_new(&(GizmoListNewDescriptor){
      .type_size = sizeof(GizmoAmbientLight),
      .capacity = &list->ambient_light.capacity,
      .length = &list->ambient_light.length,
      .entries = (void **)&list->ambient_light.entries,
  });
}

// sun light gizmo
int gizmo_list_insert_sun_light(GizmoList *list, SunLight *light,
                                GizmoMeshList *mesh_list) {

  // init new entry
  GizmoSunLight new_entry = {light};

  // copy mesh list
  if (gizmo_mesh_list_copy(mesh_list, &new_entry.meshes) != GIZMO_SUCCESS)
    return GIZMO_ALLOC_FAIL;

  // insert new entry (use abstract function gizmo_list_insert)
  return gizmo_list_insert(&(GizmoListInsertDescriptor){
      .type_size = sizeof(GizmoSunLight),
      .new_entry = &new_entry,
      .capacity = &list->sun_light.capacity,
      .length = &list->sun_light.length,
      .entries = (void **)&list->sun_light.entries,
  });
}
SunLight *gizmo_list_new_sun_light(GizmoList *list) {
  return (SunLight *)gizmo_list_new(&(GizmoListNewDescriptor){
      .type_size = sizeof(GizmoSunLight),
      .capacity = &list->sun_light.capacity,
      .length = &list->sun_light.length,
      .entries = (void **)&list->sun_light.entries,
  });
}

// spot light gizmo
int gizmo_list_insert_spot_light(GizmoList *list, SpotLight *light,
                                 GizmoMeshList *mesh_list) {

  // init new entry
  GizmoSpotLight new_entry = {light};

  // copy mesh list
  if (gizmo_mesh_list_copy(mesh_list, &new_entry.meshes) != GIZMO_SUCCESS)
    return GIZMO_ALLOC_FAIL;

  // insert new entry (use abstract function gizmo_list_insert)
  return gizmo_list_insert(&(GizmoListInsertDescriptor){
      .type_size = sizeof(GizmoSpotLight),
      .new_entry = &new_entry,
      .capacity = &list->spot_light.capacity,
      .length = &list->spot_light.length,
      .entries = (void **)&list->spot_light.entries,
  });
}
SpotLight *gizmo_list_new_spot_light(GizmoList *list) {
  return (SpotLight *)gizmo_list_new(&(GizmoListNewDescriptor){
      .type_size = sizeof(GizmoSpotLight),
      .capacity = &list->spot_light.capacity,
      .length = &list->spot_light.length,
      .entries = (void **)&list->sun_light.entries,
  });
}

// camera gizmo
int gizmo_list_insert_camera(GizmoList *list, Camera *camera,
                             GizmoMeshList *mesh_list) {

  // init new entry
  GizmoCamera new_entry = {camera};

  // copy mesh list
  if (gizmo_mesh_list_copy(mesh_list, &new_entry.meshes) != GIZMO_SUCCESS)
    return GIZMO_ALLOC_FAIL;

  // insert new entry (use abstract function gizmo_list_insert)
  return gizmo_list_insert(&(GizmoListInsertDescriptor){
      .type_size = sizeof(GizmoCamera),
      .new_entry = &new_entry,
      .capacity = &list->camera.capacity,
      .length = &list->camera.length,
      .entries = (void **)&list->camera.entries,
  });
}
Camera *gizmo_list_new_camera(GizmoList *list) { return NULL; }
