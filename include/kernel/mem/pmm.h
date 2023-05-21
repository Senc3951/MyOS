#pragma once

#include "mmu.h"
#include <stdbool.h>

#define PAGE_SIZE 4096

struct pmm_info
{
    uint64_t start;
    uint64_t blocks;
    uint64_t *buffer;
};

/**
 * @brief Initialize the Physical memory manager.
 * 
 * @param region Largest physical memory region.
*/
void pmm_init(struct memory_region *region);

/**
 * @brief Get the total amount of memory.
 * 
 * @return Total amount of memory.
*/
uint64_t pmm_get_total();

/**
 * @brief Get the amount of free memory.
 * 
 * @return Amount of free memory.
*/
uint64_t pmm_get_free();

/**
 * @brief Get the amount of used memory.
 * 
 * @return Amount of used memory.
*/
uint64_t pmm_get_used();

/**
 * @brief Allocate memory blocks.
 * 
 * @param blocks Amount of blocks to allocate.
 * @return Allocated blocks if found available ones, NULL otherwise.
*/
void *pmm_alloc(const size_t blocks);

/**
 * @brief Free memory blocks.
 * 
 * @param address Address of allocated blocks.
 * @param blocks Size of allocated blocks.
 * @return 1 if successfully free'd the blocks, False otherwise.
*/
bool pmm_free(void *address, const size_t blocks);