#pragma once

#include <common.h>

enum
{
    GDT_ACCESS_ACCESSED         = 1 << 0,
    GDT_ACCESS_READ_WRITE       = 1 << 1,
    GDT_ACCESS_DIRECTION        = 1 << 2,
    GDT_ACCESS_EXECUTABLE       = 1 << 3,
    GDT_ACCESS_TYPE             = 1 << 4,
    GDT_ACCESS_DPL0             = 0 << 5,
    GDT_ACCESS_DPL3             = 3 << 5,
    GDT_ACCESS_PRESENT          = 1 << 7
};

enum
{
    TSS_ACCESS_TYPE32_AVAILABLE = 0b1001,
    TSS_ACCESS_DPL0             = 0 << 5,
    TSS_ACCESS_DPL3             = 3 << 5,
    TSS_ACCESS_PRESENT          = 1 << 7
};

enum
{
    GDT_FLAGS_LONG_MODE   = 1 << 1,
    GDT_FLAGS_SIZE        = 1 << 2,
    GDT_FLAGS_GRANULARITY = 1 << 3
};

#define GDT_ENTRIES             6

#define GDT_KERNEL_CS_ACCESS    GDT_ACCESS_READ_WRITE | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_TYPE | GDT_ACCESS_DPL0 | GDT_ACCESS_PRESENT
#define GDT_KERNEL_DS_ACCESS    GDT_ACCESS_READ_WRITE | GDT_ACCESS_TYPE | GDT_ACCESS_DPL0 | GDT_ACCESS_PRESENT
#define GDT_USER_CS_ACCESS      GDT_ACCESS_READ_WRITE | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_TYPE | GDT_ACCESS_DPL3 | GDT_ACCESS_PRESENT
#define GDT_USER_DS_ACCESS      GDT_ACCESS_READ_WRITE | GDT_ACCESS_TYPE | GDT_ACCESS_DPL3 | GDT_ACCESS_PRESENT
#define GDT_TSS_ACCESS          TSS_ACCESS_TYPE32_AVAILABLE | TSS_ACCESS_DPL0 | TSS_ACCESS_PRESENT
#define GDT_FLAGS               GDT_FLAGS_GRANULARITY | GDT_FLAGS_SIZE

typedef struct TASK_STATE_SEGMENT
{
    uint32_t previousTss;
    uint32_t esp0;  // Kernel stack
    uint32_t ss0;   // Segment used by kernel stack
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t ioMapBaseOffset;
} __PACKED__ task_state_segment_t;

typedef struct GDT_ENTRY
{
    uint16_t limitLow;
    uint16_t baseLow;
    uint8_t baseMid;
    uint8_t access;
    uint8_t limitHigh : 4;
    uint8_t flags : 4;
    uint8_t baseHigh;
} __PACKED__ gdt_entry_t;

void gdt_init();
void tss_init();