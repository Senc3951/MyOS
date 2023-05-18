#include <arch/gdt.h>

#define GET_TSS_ENTRY(addr) ({ \
    (struct tss_entry) { \
        .size = sizeof(struct tss), \
        .base_low = (uint16_t)addr & 0xffff, \
        .base_mid = (uint8_t)(addr >> 16) & 0xff, \
        .base_high = (uint8_t)(addr >> 24) & 0xff, \
        .base_upper = (uint32_t)(addr >> 32), \
        .flags1 = 0b10001001, \
        .flags2 = 0, \
        .reserved0 = 0 }; \
})

extern void x64_load_gdt(struct gdt *gdt, size_t cs, size_t ds);

struct gdt_entries g_GDTEntries;
struct tss g_TSS = {
    .reserved0 = 0,
    .rsp = {},
    .reserved1 = 0,
    .ist = {},
    .reserved2 = 0,
    .reserved3 = 0,
    .reserved4 = 0,
    .iomap_base = 0
};

static inline struct gdt_entry get_gdt_entry(const uint32_t base, const uint32_t limit, const uint8_t access, const uint8_t granularity)
{
    return (struct gdt_entry) 
    { 
        .limitLow = GDT_GET_LIMIT_LOW(limit),
        .baseLow = GDT_GET_BASE_LOW(base),
        .baseMiddle = GDT_GET_BASE_MIDDLE(base),
        .access = access,
        .granularity = GDT_GET_GRANULARITY(limit, granularity),
        .baseHigh = GDT_GET_BASE_HIGH(base) 
    };
}

void gdt_init()
{
    g_GDTEntries.entries[0] = get_gdt_entry(0, 0, 0, 0),
    g_GDTEntries.entries[1] = get_gdt_entry(0, 0, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE | GDT_EXECUTABLE, GDT_LONG_MODE_GRANULARITY),
    g_GDTEntries.entries[2] = get_gdt_entry(0, 0, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE, 0),
    g_GDTEntries.entries[3] = get_gdt_entry(0, 0, 0, 0),
    g_GDTEntries.entries[4] = get_gdt_entry(0, 0, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE | GDT_USER, GDT_LONG_MODE_GRANULARITY),
    g_GDTEntries.entries[5] = get_gdt_entry(0, 0, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE | GDT_USER, 0),
    g_GDTEntries.tss = GET_TSS_ENTRY((uintptr_t)&g_TSS);
    
    struct gdt gdt = 
    {
        .limit = sizeof(g_GDTEntries) - 1,
        .base = (uint64_t) &g_GDTEntries
    };
    
    x64_load_gdt(&gdt, KERNEL_CS, KERNEL_DS);
}