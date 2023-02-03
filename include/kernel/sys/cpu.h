#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct CPUID_FEATURE
{
    uint32_t func;
    uint32_t param;
    enum
    {
        CPUID_REG_EAX,
        CPUID_REG_EBX,
        CPUID_REG_ECX,
        CPUID_REG_EDX
    } reg;
    
    uint32_t mask;
} CPUIDFeature_t;

void cpuid(const uint32_t func, const uint32_t param, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx);
bool cpuid_CheckFeature(CPUIDFeature_t feature);