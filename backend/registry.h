#ifndef _UTILS_ID_H_
#define _UTILS_ID_H_

#define REG_MAX_OBJECTS 1024
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
} RegEntryType;

typedef struct {
  reg_id_t id;
  RegEntryType type;
  void *ptr;
} RegEntry;

static RegEntry g_reg[REG_MAX_OBJECTS] = {0};

reg_id_t reg_register(void *, RegEntryType);

void *reg_lookup(reg_id_t);

#endif
