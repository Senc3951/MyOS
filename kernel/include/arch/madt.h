#pragma once

#include <common.h>

#define MAX_CORES               128
#define MAX_IO_APIC             8
#define MAX_IO_APIC_REDIRECTS   16

typedef struct IO_APIC
{
    uint8_t id;
    uint8_t reserved;
    uint32_t address;
    uint32_t gsib;
} __PACKED__ ioapic_t;

typedef struct MADT_INFO
{
    uint8_t processorID[MAX_CORES];
    ioapic_t *ioapic[MAX_IO_APIC];
    uint64_t lapic;
    size_t processorCount, ioapicCount;
} madt_t;

void madt_init();

extern madt_t _MADT;