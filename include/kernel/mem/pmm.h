#pragma once

#include "mmu.h"
#include <stdbool.h>

#define BLOCK_SIZE 4096

typedef struct PMM_INFO
{
    uint64_t start;
    uint64_t blocks;
    uint32_t *buffer;
} PMMInfo_t;

/**
 * @brief Initialize the Physical memory manager.
 * 
 * @param region Largest physical memory region.
*/
void PMM_Initialize(MemoryRegion_t *region);

/**
 * @brief Get the total amount of memory.
 * 
 * @return Total amount of memory.
*/
uint64_t PMM_GetTotal();

/**
 * @brief Get the amount of free memory.
 * 
 * @return Amount of free memory.
*/
uint64_t PMM_GetFree();

/**
 * @brief Get the amount of used memory.
 * 
 * @return Amount of used memory.
*/
uint64_t PMM_GetUsed();

/**
 * @brief Allocate a memory chunk of size 'BLOCK_SIZE'.
 * 
 * @return Allocated chunk if successfully found a block, NULL otherwise.
*/
void *PMM_AllocateBlock();

/**
 * @brief Allocate memory blocks.
 * 
 * @param blocks Amount of blocks to allocate.
 * @return Allocated blocks if found available ones, NULL otherwise.
*/
void *PMM_Allocate(const size_t blocks);

/**
 * @brief Allocate and clear memory blocks.
 * 
 * @param blocks Amount of blocks to allocate.
 * @return Allocated blocks if found available ones, NULL otherwise.
*/
void *PMM_Calloc(const size_t blocks);

/**
 * @brief Free a memory block.
 * 
 * @param address Address of allocated block.
 * @return 1 if successfully free'd the block, False otherwise.
*/
bool PMM_FreeBlock(void *address);

/**
 * @brief Free memory blocks.
 * 
 * @param address Address of allocated blocks.
 * @param blocks Size of allocated blocks.
 * @return 1 if successfully free'd the blocks, False otherwise.
*/
bool PMM_Free(void *address, const size_t blocks);