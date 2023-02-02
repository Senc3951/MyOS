#pragma once

#include <stdint.h>

typedef struct SYMBOL
{
    uint64_t addr;
    char *name;
} __attribute__((packed)) symbol_t;

extern const symbol_t _KernelSymbols[];