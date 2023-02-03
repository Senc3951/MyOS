#pragma once

#include <stdint.h>
#include <stddef.h>

#define MMU_MAX_REGIONS 64
#define MMU_CMD_ARGS_MAX 256
#define MMU_BOOTLOADER_NAME_MAX 64
#define MMU_MAX_MODULES 20

typedef enum MemoryEntryType
{
    MMAP_Free = 1,
    MMAP_Reserved,
    MMAP_ACPI_Reclaimable,
    MMAP_NVS,
    MMAP_BADRAM
} MemoryEntryType_t;

typedef struct MEMORY_REGION
{
    uintptr_t start;
    uint64_t len;
    MemoryEntryType_t memoryEntryType;
} MemoryRegion_t;

typedef struct MEMORY_MODULE
{
    uintptr_t start;
    uint64_t len;
    char cmdArgs[MMU_CMD_ARGS_MAX];
} MemoryModule_t;

typedef struct MEMORY_MAP
{
    uint8_t regions;
    uint8_t modulesCount;
    MemoryRegion_t physicalRegions[MMU_MAX_REGIONS];
    MemoryModule_t modules[MMU_MAX_MODULES];
    char cmdArgs[MMU_CMD_ARGS_MAX];
    char bootloaderName[MMU_BOOTLOADER_NAME_MAX];
} MemoryMap_t;

typedef struct MB2_INFO
{
    uint32_t totalSize;
    uint32_t reserved;
} MB2Info_t;

/**
 * @brief Initialize the MMU.
 * 
 * @param magic Magic number associated with the bootloader.
 * @param mbTags Information about the memory passed by the bootloader.
*/
void MMU_Initialize(const uintptr_t magic, const MB2Info_t *mbTags);

/**
 * @brief Print all known information about the memory.
*/
void MMU_PrintDebug();

/**
 * @brief Get the largest region available.
 * 
 * @return Largest region avaiable.
*/
MemoryRegion_t *MMU_LargestRegion();