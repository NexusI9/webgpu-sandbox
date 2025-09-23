#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <stdint.h>
#include <string.h>
#include <webgpu/webgpu.h>

#define LOGGER_MAX_ENTRY 2048
#define LOGGER_ENTRY_TYPE_COUNT 12
#define LOGGER_ENTRY_MESSAGE_LENGTH 256

typedef enum {
  LoggerFlag_Print,
  LoggerFlag_Info,
  LoggerFlag_Error,
  LoggerFlag_Warning,
  LoggerFlag_Debug,
  LoggerFlag_ShaderCreate,
  LoggerFlag_MeshBuild,
  LoggerFlag_MeshCreate,
  LoggerFlag_Import,
  LoggerFlag_Success,
  LoggerFlag_Process,
  LoggerFlag_Header,
} LoggerFlag;

typedef struct {
  char messages[LOGGER_MAX_ENTRY][LOGGER_ENTRY_MESSAGE_LENGTH];
  uint64_t timestamps[LOGGER_MAX_ENTRY];
  LoggerFlag flags[LOGGER_MAX_ENTRY];
  size_t length;
} Logger;

extern Logger g_logger;

static inline void logger_add(const char *message, const LoggerFlag flag) {

  size_t index = (g_logger.length++) % LOGGER_MAX_ENTRY;

  g_logger.flags[index] = flag;
  strncpy(g_logger.messages[index], message, LOGGER_ENTRY_MESSAGE_LENGTH - 1);
  g_logger.messages[index][LOGGER_ENTRY_MESSAGE_LENGTH - 1] = '\0';

  
}

#endif
