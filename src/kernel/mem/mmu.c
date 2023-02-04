#include <kernel/mem/mmu.h>
#include <kernel/mem/pmm.h>
#include <kernel/sys/panic.h>
#include <kernel/sys/logger.h>
#include <kernel/sys/assert.h>
#include <kernel/lib/string.h>
#include <kernel/arch/acpi/acpi.h>

#define ALIGN_SIZE(size) ((size + 7) & ~7)

MemoryMap_t g_MMAP;

static void parseMemory(const uintptr_t magic, const MB2Info_t *mbTags);
static void parseACPIV1(struct multiboot_tag_old_acpi *tag);
static void parseACPIV2(struct multiboot_tag_new_acpi *tag);
static void parseMMAP(const struct multiboot_tag_mmap *tag);
static void parseFrameBuffer(struct multiboot_tag_framebuffer *tag);

static inline void printFormattedBytes(const size_t bytes)
{
    if (bytes > 1073741824)
        LOGINM("%ld GB", bytes / 1073741824);
    else if (bytes > 1048576)
        LOGINM("%ld MB", bytes / 1048576);
    else if (bytes > 1024)
        LOGINM("%ld KB", bytes / 1024);
    else
        LOGINM("%ld B", bytes);
}

static inline void printFormattedMemoryType(const MemoryEntryType_t type)
{
    switch (type)
    {
        case MMAP_Free:
            LOGINM("Free");
            break;
        case MMAP_Reserved:
            LOGINM("Reserved");
            break;
        case MMAP_ACPI_Reclaimable:
            LOGINM("ACPI Reclaimable");
            break;
        case MMAP_NVS:
            LOGINM("NVS");
            break;
        case MMAP_BADRAM:
            LOGINM("BADRAM");
            break;
        default:
            LOGINM("Unknown");
            break;
    }
}

void MMU_Initialize(const uintptr_t magic, const MB2Info_t *mbTags)
{
    g_MMAP.regions = 0;
    g_MMAP.modulesCount = 0;
    memset(g_MMAP.cmdArgs, 0, MMU_CMD_ARGS_MAX);
    memset(g_MMAP.bootloaderName, 0, MMU_BOOTLOADER_NAME_MAX);
    
    parseMemory(magic, mbTags);
    PMM_Initialize(MMU_LargestRegion());
        
    // Initialize virtual memory (paging)
}

void parseMemory(const uintptr_t magic, const MB2Info_t *mbTags)
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
                LOGI("MMU", "Command line args: [%s]\n", g_MMAP.cmdArgs);
                break;
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                strncpy(g_MMAP.bootloaderName, ((struct multiboot_tag_string *)tag)->string, MMU_BOOTLOADER_NAME_MAX);
                LOGI("MMU", "Boot loader name: [%s]\n", g_MMAP.bootloaderName);
                break;
            case MULTIBOOT_TAG_TYPE_MODULE:
                if (i >= MMU_MAX_MODULES)
                {
                    LOGW("MMU", "Modules exceeded max (%d)\n", MMU_MAX_MODULES);
                    continue;
                }
                
                struct multiboot_tag_module *mod = ((struct multiboot_tag_module *)tag);
                g_MMAP.modules[i].start = mod->mod_start;
                g_MMAP.modules[i].len = mod->mod_end - mod->mod_start;
                strncpy(g_MMAP.modules[i++].cmdArgs, mod->cmdline, MMU_CMD_ARGS_MAX);
                
                LOGI("MMU", "Module at: 0x%x - 0x%x. Command line args: [%s]\n", mod->mod_start, mod->mod_end, mod->cmdline);
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_OLD:
                foundACPI = true;
                LOGI("MMU", "ACPI V1 Detected.\n");
    
                parseACPIV1((struct multiboot_tag_old_acpi *)tag);
                break;
            case MULTIBOOT_TAG_TYPE_ACPI_NEW:
                foundACPI = true;
                LOGI("MMU", "ACPI V2 Detected.\n");
                
                parseACPIV2((struct multiboot_tag_new_acpi *)tag);
                break;
            case MULTIBOOT_TAG_TYPE_MMAP:
                parseMMAP((struct multiboot_tag_mmap *)tag);
                break;
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                parseFrameBuffer((struct multiboot_tag_framebuffer *)tag);
            default:
                break;
        }
    }
    
    g_MMAP.modulesCount = i;
    ASSERT(foundACPI, "ACPI Could not be found.\n");
}

void parseACPIV1(struct multiboot_tag_old_acpi *tag)
{
    if (ACPI_ParseV1(tag))
        ACPI_Enable();
}

void parseACPIV2(struct multiboot_tag_new_acpi *tag)
{
    if (ACPI_ParseV2(tag))
        ACPI_Enable();
}

void parseMMAP(const struct multiboot_tag_mmap *tag)
{
    uint8_t i = 0;
    multiboot_memory_map_t* entry = (multiboot_memory_map_t*)(tag->entries);
    
    while ((uint8_t*)entry < (uint8_t *)tag + tag->size)
    {
        if (i >= MMU_MAX_REGIONS)
        {
            LOGW("MMU", "Physical regions exceeded max (%d)\n", MMU_MAX_REGIONS);
            return;
        }
        
        g_MMAP.physicalRegions[i].start = entry->addr;
        g_MMAP.physicalRegions[i].len = entry->len;
        g_MMAP.physicalRegions[i++].memoryEntryType = entry->type;
        
        LOGI("MMU", "Start: 0x%x, Length: %lu B (", entry->addr, entry->len);
        printFormattedBytes(entry->len);
        LOGINM("), Type: ");
        printFormattedMemoryType(entry->type);
        LOGINM("\n");
        
        entry = (multiboot_memory_map_t *)((uintptr_t)entry + tag->entry_size);
    }
    
    g_MMAP.regions = i;
}

void parseFrameBuffer(struct multiboot_tag_framebuffer *tag)
{
    uint64_t addr = tag->common.framebuffer_addr;
    uint32_t width = tag->common.framebuffer_width;
    uint32_t height = tag->common.framebuffer_height;
    uint32_t pitch = tag->common.framebuffer_pitch;
    
    LOGE("MMU", "TODO: Implement frame buffer.\n");
}

MemoryRegion_t *MMU_LargestRegion()
{
    size_t largestSize = 0, tmp = 0;
    MemoryRegion_t *largestRegion;
    
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