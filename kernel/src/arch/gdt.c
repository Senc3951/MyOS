#include <arch/gdt.h>
#include <arch/cpu.h>
#include <libc/string.h>
#include <log.h>

static __ALIGNED__(sizeof(uintptr_t)) gdt_entry_t g_entries[GDT_ENTRIES];
static __ALIGNED__(sizeof(uintptr_t)) task_state_segment_t g_tss = { 0 };
static __ALIGNED__(sizeof(uintptr_t)) table_descriptor_t g_descriptor;
static size_t g_index = 0, g_tssIndex;

static inline void addEntry(const uint32_t base, const uint32_t limit, const uint8_t access, const uint8_t flags)
{
    gdt_entry_t *entry = &g_entries[g_index++];

    entry->limitLow = (uint16_t)(limit & 0xFFFF);
    entry->baseLow = (uint16_t)(base & 0xFFFF);
    entry->baseMid = (uint8_t)((base >> 16) & 0xFF);
    entry->access = access;
    entry->limitHigh = (uint8_t)((limit >> 16) & 0xF);
    entry->flags = flags & 0xF;
    entry->baseHigh = (uint8_t)((base >> 24) & 0xFF);
}

void gdt_init()
{
    g_tss.ioMapBaseOffset = sizeof(task_state_segment_t);
    
    // Add segment descriptors
    addEntry(0, 0, 0, 0);                                     // Null descriptor
    addEntry(0, 0xFFFF, GDT_KERNEL_CS_ACCESS, GDT_FLAGS);     // Kernel code descriptor
    addEntry(0, 0xFFFF, GDT_KERNEL_DS_ACCESS, GDT_FLAGS);     // Kernel data descriptor
    addEntry(0, 0xFFFF, GDT_USER_CS_ACCESS, GDT_FLAGS);       // User code descriptor
    addEntry(0, 0xFFFF, GDT_USER_DS_ACCESS, GDT_FLAGS);       // User data descriptor
    
    // Add tss descriptor
    g_tssIndex = g_index;
    uint32_t tssAddress = (uint32_t)&g_tss;
    addEntry(tssAddress, sizeof(g_tss) - 1, GDT_TSS_ACCESS, 0);
    
    // Load gdt
    g_descriptor.size = (uint16_t)(sizeof(g_entries) - 1);
    g_descriptor.offset = (uint32_t)g_entries;
    uintptr_t gdtAddress = (uintptr_t)&g_descriptor;
    load_gdt(gdtAddress);
    
    LOG("GDT loaded at %p\n", gdtAddress);
}

void tss_init()
{
    load_tss(g_tssIndex * sizeof(gdt_entry_t));
    LOG("TSS struct at %p. TSS located at entry %u in GDT\n", &g_tss, g_tssIndex);
}