#pragma once

#include <stdint.h>
#include <stddef.h>
#include <boot/mb2.h>

#define MMU_MAX_REGIONS 64
#define MMU_CMD_ARGS_MAX 256
#define MMU_BOOTLOADER_NAME_MAX 64
#define MMU_MAX_MODULES 20

enum memory_entry_type
{
    MMAP_Free = 1,
    MMAP_Reserved,
    MMAP_ACPI_Reclaimable,
    MMAP_NVS,
    MMAP_BADRAM
};

struct memory_region
{
    uintptr_t start;
    uint64_t len;
    enum memory_entry_type memoryEntryType;
};

struct memory_module
{
    uintptr_t start;
    uint64_t len;
    char cmdArgs[MMU_CMD_ARGS_MAX];
};

struct memory_map
{
    uint8_t regions;
    uint8_t modulesCount;
    struct memory_region physicalRegions[MMU_MAX_REGIONS];
    struct memory_module modules[MMU_MAX_MODULES];
    char cmdArgs[MMU_CMD_ARGS_MAX];
    char bootloaderName[MMU_BOOTLOADER_NAME_MAX];
};

/**
 * @brief Initialize the MMU.
 * 
 * @param magic Magic number associated with the bootloader.
 * @param mbTags Information about the memory passed by the bootloader.
*/
void mmu_init(const uintptr_t magic, const struct multiboot2_info *mbTags);

/**
 * @brief Get the largest region available.
 * 
 * @return Largest region avaiable.
*/
struct memory_region *mmu_largest_region();