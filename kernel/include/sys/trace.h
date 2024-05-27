#pragma once

#include <common.h>

#define MAX_TRACE 20

uint8_t stacktrace(uintptr_t *addrptrs, const uint8_t max);
void stacktrace_dump();