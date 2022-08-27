#ifndef DEBUG_H 
#define DEBUG_H

#include <stdbool.h>

extern bool DEBUG;

void DBG_log(const char* format, ...);

#endif