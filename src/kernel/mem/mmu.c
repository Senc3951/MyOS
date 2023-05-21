#include <stddef.h>
#include <mem/mmu.h>
#include <mem/pmm.h>
#include <sys/panic.h>
#include <sys/logger.h>
#include <sys/assert.h>
#include <lib/string.h>
#include <arch/acpi/acpi.h>

#define ALIGN_SIZE(size) ((size + 7) & ~7)

struct memory_map g_MMAP;

static inline void print_formatted_bytes(const size_t bytes)
{
    if (bytes > 1073741824)
        LOGN("%f GB", (double)bytes / 1073741824);
    else if (bytes > 1048576)
        LOGN("%f MB", (double)bytes / 1048576);
    else if (bytes > 1024)
        LOGN("%f KB", (double)bytes / 1024);
    else
        LOGN("%ld B", bytes);
}

static inline void print_formatted_memory_type(const enum memory_entry_type type)
{
    switch (type)
    {
        case MMAP_Free:
            LOGN("Free");
            break;
        case MMAP_Reserved:
            LOGN("Reserved");
            break;
        case MMAP_ACPI_Reclaimable:
            LOGN("ACPI Reclaimable");
            break;
        case MMAP_NVS:
            LOGN("NVS");
            break;
        case MMAP_BADRAM:
            LOGN("BADRAM");
            break;
        default:
            LOGN("Unknown");
            break;
    }
}

static inline void parse_acpi_old(struct multiboot_tag_old_acpi *tag)
{
    if (acpi_parse_old(tag))
        acpi_enable();
}

static inline void parse_acpi_new(struct multiboot_tag_new_acpi *tag)
{
    if (acpi_parse_new(tag))
        acpi_enable();
}

static void parse_mmap(const struct multiboot_tag_mmap *tag)
{
    uint8_t i = 0;
    multiboot_memory_map_t* entry = (multiboot_memory_map_t*)(tag->entries);
    
    while ((uint8_t*)entry < (uint8_t *)tag + tag->size)
    {
        if (i >= MMU_MAX_REGIONS)
        {
            LOG("Physical regions exceeded max (%d)\n", MMU_MAX_REGIONS);
            return;
        }
        
        g_MMAP.physicalRegions[i].start = entry->addr;
        g_MMAP.physicalRegions[i].len = entry->len;
        g_MMAP.physicalRegions[i++].memoryEntryType = entry->type;
        
        LOG("Memory Chunk Start: 0x%x, Length: %lu B (", entry->addr, entry->len);
        print_formatted_bytes(entry->len);
        LOGN("), Type: ");
        print_formatted_memory_type(entry->type);
        LOGN("\n");
        
        entry = (multiboot_memory_map_t *)((uintptr_t)entry + tag->entry_size);
    }
    
    g_MMAP.regions = i;
}

static void parse_memory(const uintptr_t magic, const struct multiboot2_info *mbTags)
{
    ASSERT(magic == MULTIBOOT2_BOOTLOADER_MAGIC, "Invalid magic number, expected: 0x%x, got: 0x%x\n", MULTIBOOT2_BOOTLOADER_MAGIC, magic);
    if (*(uintptr_t *)mbTags & 7)
        KPANIC("Unaligned MBI: 0x%x\n", mbTags);
    
    uint16_t i = 0;
    bool foundACPI = false;
    struct multiboot_tag *tag;
    
    for (tag = (struct multiboot_tag *)(mbTags + 1); tag->type != MULTIBOOT_TAG_TYPE_END; tag = (struct multiboot_tag *)((uint8_t *)tag + ALIGN_SIZE(tag->size)))
    {
        switch (tag->type)
        {
            case MULTIBOOT_TAG_TYPE_CMDLINE:
                strncpy(g_MMAP.cmdArgs, ((struct multiboot_tag_string *)tag)->string, MMU_CMD_ARGS_MAX);
                LOG("Command line args: [%s]\n", g_MMAP.cmdArgs);
                break;
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                strncpy(g_MMAP.bootloaderName, ((struct multiboot_tag_string *)tag)->string, MMU_BOOTLOADER_NAME_MAX);
                LOG("Bootloader name: [%s]\n", g_MMAP.bootloaderName);
                break;
            case MULTIBOOT_TAG_TYPE_MODULE:
                if (i >= MMU_MAX_MODULES)
                {
                    LOG("Modules exceeded max (%d)\n", MMU_MAX_MODULES);
                    continue;
                }
                
                struct multiboot_tag_module *mod = ((struct multiboot_tag_module *)tag);
                g_MMAP.modules[i].start = mod->mod_start;
                g_MMAP.modules[i].len = mod->mod_end - mod->mod_start;
                strncpy(g_MMAP.modules[i++].cmdArgs, mod->cmdline, MMU_CMD_ARGS_MAX);
                
                LOG("Module at: 0x%x - 0x%x. Command line args: [%s]\n", mod->mod_start, mod->mod_end, mod->cmdline);
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_OLD:
                foundACPI = true;
                LOG("ACPI V1 Detected.\n");
    
                parse_acpi_old((struct multiboot_tag_old_acpi *)tag);
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_NEW:
                foundACPI = true;
                LOG("ACPI V2 Detected.\n");
                
                parse_acpi_new((struct multiboot_tag_new_acpi *)tag);
                break;
            case MULTIBOOT_TAG_TYPE_MMAP:
                parse_mmap((struct multiboot_tag_mmap *)tag);
                break;
            default:
                break;
        }
    }
    
    g_MMAP.modulesCount = i;
    ASSERT(foundACPI, "ACPI Could not be found.\n");
}

void mmu_init(const uintptr_t magic, const struct multiboot2_info *mbTags)
{
    g_MMAP.regions = 0;
    g_MMAP.modulesCount = 0;
    memset(g_MMAP.cmdArgs, 0, MMU_CMD_ARGS_MAX);
    memset(g_MMAP.bootloaderName, 0, MMU_BOOTLOADER_NAME_MAX);
    
    parse_memory(magic, mbTags);
    pmm_init(mmu_largest_region());
}

struct memory_region *mmu_largest_region()
{
    size_t largestSize = 0, tmp = 0;
    struct memory_region *largestRegion;
    
    for (size_t i = 0; i < g_MMAP.regions; i++)
    {
        tmp = g_MMAP.physicalRegions[i].len;
        if (g_MMAP.physicalRegions[i].memoryEntryType == MMAP_Free && tmp > largestSize)
        {
            largestSize = tmp;
            largestRegion = &g_MMAP.physicalRegions[i];
        }
    }
    
    return largestRegion;
}