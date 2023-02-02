#include <kernel/arch/gdt.h>

extern void x64_load_gdt(GlobalDescriptorTable_t *gdt, size_t cs, size_t ds);

static inline GDTEntry_t getGDTEntry(const uint32_t base, const uint32_t limit, const uint8_t access, const uint8_t granularity)
{
    GDTEntry_t entry = 
    {
        .limitLow = GET_GDT_LIMIT_LOW(limit),
        .baseLow = GET_GDT_BASE_LOW(base),
        .baseMiddle = GET_GDT_BASE_MIDDLE(base),
        .access = access,
        .granularity = GET_GDT_GRANULARITY(limit, granularity),
        .baseHigh = GET_GDT_BASE_HIGH(base)
    };
    return entry;
}

static inline TSSEntry_t getTSSEntry(const uintptr_t addr)
{
    TSSEntry_t entry = 
    {
        .size = sizeof(TaskStateSegment_t),
        .base_low = (uint16_t)addr & 0xffff,
        .base_mid = (uint8_t)(addr >> 16) & 0xff,
        .base_high = (uint8_t)(addr >> 24) & 0xff,
        .base_upper = (uint32_t)(addr >> 32),
        .flags1 = 0b10001001,
        .flags2 = 0,
        .reserved0 = 0
    };
    return entry;
}

GDTEntries_t g_GDTEntries;
TaskStateSegment_t g_TSS = {
    .reserved0 = 0,
    .rsp = {},
    .reserved1 = 0,
    .ist = {},
    .reserved2 = 0,
    .reserved3 = 0,
    .reserved4 = 0,
    .iomap_base = 0
};

void GDT_Initialize()
{
    g_GDTEntries.entries[0] = getGDTEntry(0, 0, 0, 0),
    g_GDTEntries.entries[1] = getGDTEntry(0, 0, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE | GDT_EXECUTABLE, GDT_LONG_MODE_GRANULARITY),
    g_GDTEntries.entries[2] = getGDTEntry(0, 0, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE, 0),
    g_GDTEntries.entries[3] = getGDTEntry(0, 0, 0, 0),
    g_GDTEntries.entries[4] = getGDTEntry(0, 0, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE | GDT_USER, GDT_LONG_MODE_GRANULARITY),
    g_GDTEntries.entries[5] = getGDTEntry(0, 0, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE | GDT_USER, 0),
    g_GDTEntries.tss = getTSSEntry((uintptr_t)&g_TSS);
    
    GlobalDescriptorTable_t gdt = 
    {
        .limit = sizeof(g_GDTEntries) - 1,
        .base = (uint64_t) &g_GDTEntries
    };
    
    x64_load_gdt(&gdt, KERNEL_CS, KERNEL_DS);
}