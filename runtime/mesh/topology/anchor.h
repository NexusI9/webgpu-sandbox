#ifndef _MESH_TOPOLOGY_ANCHOR_H_
#define _MESH_TOPOLOGY_ANCHOR_H_

#include "core.h"

#define MESH_TOPOLOGY_ANCHOR_LIST_DEFAULT_CAPACITY 1023
#define MESH_TOPOLOGY_ANCHOR_DEFAULT_CAPACITY 32

#define MESH_TOPOLOGY_ANCHOR_SUCCESS 0
#define MESH_TOPOLOGY_ANCHOR_ALLOC_FAIL 1
#define MESH_TOPOLOGY_ANCHOR_ERROR 2
#define MESH_TOPOLOGY_ANCHOR_UNSET 3

typedef union {
  vattr_t f;
  uint32_t u;
} MeshTopologyAnchorKey;

typedef struct {
  vindex_t *entries;
  size_t length;
  size_t capacity;
} MeshTopologyAnchor;

typedef struct {
  MeshTopologyAnchor *entries;
  size_t length;
  size_t capacity;
} MeshTopologyAnchorList;


/**
     ▗▄▖ ▗▖  ▗▖ ▗▄▄▖▗▖ ▗▖ ▗▄▖ ▗▄▄▖
    ▐▌ ▐▌▐▛▚▖▐▌▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌
    ▐▛▀▜▌▐▌ ▝▜▌▐▌   ▐▛▀▜▌▐▌ ▐▌▐▛▀▚▖
    ▐▌ ▐▌▐▌  ▐▌▝▚▄▄▖▐▌ ▐▌▝▚▄▞▘▐▌ ▐▌

 */
int mesh_topology_anchor_create(MeshTopologyAnchor *, size_t);
int mesh_topology_anchor_expand(MeshTopologyAnchor *);
int mesh_topology_anchor_insert(MeshTopologyAnchor *, vindex_t *, size_t);
void mesh_topology_anchor_print(MeshTopologyAnchor *);

/**
     ▗▄▖ ▗▖  ▗▖ ▗▄▄▖▗▖ ▗▖ ▗▄▖ ▗▄▄▖     ▗▖   ▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖
    ▐▌ ▐▌▐▛▚▖▐▌▐▌   ▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌    ▐▌     █  ▐▌     █
    ▐▛▀▜▌▐▌ ▝▜▌▐▌   ▐▛▀▜▌▐▌ ▐▌▐▛▀▚▖    ▐▌     █   ▝▀▚▖  █
    ▐▌ ▐▌▐▌  ▐▌▝▚▄▄▖▐▌ ▐▌▝▚▄▞▘▐▌ ▐▌    ▐▙▄▄▖▗▄█▄▖▗▄▄▞▘  █

 */
uint32_t mesh_topology_anchor_list_hash_float(float);
uint32_t mesh_topology_anchor_list_hash(vertex_position);
int mesh_topology_anchor_list_expand(
    MeshTopologyAnchorList *);
int mesh_topology_anchor_list_create(
    MeshTopologyAnchorList *, size_t);

int mesh_topology_anchor_list_insert(
    MeshTopologyAnchorList *, vertex_position *, vindex_t *, size_t);

// hash based query
MeshTopologyAnchor *
mesh_topology_anchor_list_new_hash(MeshTopologyAnchorList *,
                                             vertex_position *);

MeshTopologyAnchor *
mesh_topology_anchor_list_find_hash(MeshTopologyAnchorList *,
                                              vertex_position *);
// index based query
MeshTopologyAnchor *
mesh_topology_anchor_list_new_index(MeshTopologyAnchorList *,
                                              vindex_t);

#endif
