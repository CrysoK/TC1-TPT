#include "debug.h"
#include <stdio.h>
#include <stdarg.h>

void DBG_log(const char* fmt, ...) {
  if(!DEBUG) return;
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}
