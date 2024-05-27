#pragma once

#include <common.h>

typedef struct SYMBOL
{
    uintptr_t addr;
    char *name;
} __PACKED__ symbol_t;

//extern const symbol_t _kernel_symbols[];