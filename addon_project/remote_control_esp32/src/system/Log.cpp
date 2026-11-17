#include "Log.h"
#include <stdarg.h>

static volatile bool debugEnabled = false;

void logSetDebug(bool enabled)
{
  debugEnabled = enabled;
}

bool logDebugEnabled()
{
  return debugEnabled;
}

void logInfo(const String &msg)
{
  if (debugEnabled)
    Serial.println(msg);
}

void logInfoRaw(const String &msg)
{
  if (debugEnabled)
    Serial.print(msg);
}

static void vprint(const char *format, va_list args)
{
  char buf[256];
  vsnprintf(buf, sizeof(buf), format, args);
  Serial.print(buf);
}

void logInfof(const char *format, ...)
{
  if (!debugEnabled)
    return;
  va_list args;
  va_start(args, format);
  vprint(format, args);
  va_end(args);
}

void logWarning(const String &msg)
{
  Serial.println(msg);
}

void logWarningf(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  vprint(format, args);
  va_end(args);
}
