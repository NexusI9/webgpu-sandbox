#include "logger.h"
#include <stdio.h>
#include <string.h>

Logger g_logger = {0};

typedef void (*verbose_callback)(const char *);
static inline void verbose_print(const LoggerFlag, const char *);

static const char *verbose_prefix[LOGGER_FLAG_COUNT] = {
    [LoggerFlag_Print] = "",
    [LoggerFlag_Info] = "ℹ️ ",
    [LoggerFlag_Error] = "🛑 ",
    [LoggerFlag_Warning] = "⚠️ ",
    [LoggerFlag_Debug] = "",
    [LoggerFlag_ShaderCreate] = "🎨 Creating shader: ",
    [LoggerFlag_MeshBuild] = "🧱 Building mesh: ",
    [LoggerFlag_MeshCreate] = "🔼 Creating mesh: ",
    [LoggerFlag_Import] = "📦 Importing: ",
    [LoggerFlag_Success] = "☑️ ",
    [LoggerFlag_Process] = "⚙️ ",
};

/*
  Use system printf.
 */
void verbose_print(const LoggerFlag flag, const char *message) {
  VERBOSE_PRINT("%s%s",verbose_prefix[flag], message);
}

void logger_add(const LoggerFlag flag, const char *fmt, ...) {

  size_t index = (g_logger.length++) % LOGGER_MAX_ENTRY;

  g_logger.flags[index] = flag;

  va_list args;
  va_start(args, fmt);
  vsnprintf(g_logger.messages[index], LOGGER_MESSAGE_LENGTH, fmt, args);
  va_end(args);

  verbose_print(flag, g_logger.messages[index]);
}
