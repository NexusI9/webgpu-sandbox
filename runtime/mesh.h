#ifndef _MESH_H_
#define _MESH_H_
#include <stdint.h>

// Builder Pattern | Descriptor Pattern
typedef struct {

  struct {
    const float *data;
    uint16_t length;
  } vertex;

  struct {
    const uint16_t *data;
    uint16_t length;
  } index;

} MeshCreateDescriptor;

typedef struct {

  uint8_t id;

  // vertex list
  struct {
    const float *data;
    uint16_t length;
  } vertex;

  // index list
  struct {
    const uint16_t *data;
    uint16_t length;
  } index;

  struct {

  } buffer;

} mesh;

mesh mesh_create(const MeshCreateDescriptor *);

#endif
