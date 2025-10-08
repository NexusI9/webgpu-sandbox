#ifndef _UTILS_ID_H_
#define _UTILS_ID_H_

#include "backend/logger.h"
#define REG_MAX_OBJECTS 2048
#define REG_OWNER_UNDEFINED -1

typedef int reg_id_t;
typedef enum {
  RegEntryType_Mesh,
  RegEntryType_Camera,
  RegEntryType_AmbientLight,
  RegEntryType_PointLight,
  RegEntryType_SpotLight,
  RegEntryType_SunLight,
  RegEntryType_Scene,
  RegEntryType_SceneLayer,
  RegEntryType_ProbeReflectionPlane,
  RegEntryType_ProbeReflectionGrid,
  RegEntryType_SceneEditorMesh,
  RegEntryType_SceneEditorMeshList,
} RegEntryType;

typedef struct {
  reg_id_t id;
  RegEntryType type;
  void *ptr;
} RegEntry;

#ifdef __cplusplus
extern "C" {
#endif

extern RegEntry g_reg[REG_MAX_OBJECTS];

reg_id_t reg_register(void *, RegEntryType);

static inline const RegEntry *reg_lookup(reg_id_t id) {
  if (id >= REG_MAX_OBJECTS) {
    logger_add(LoggerFlag_Error, "id out of registry bounds.");
    return NULL;
  }

  return &g_reg[id];
}

#ifdef __cplusplus
}
#endif

#endif
