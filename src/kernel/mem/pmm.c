#include <kernel/mem/pmm.h>
#include <kernel/sys/panic.h>
#include <kernel/lib/string.h>

PMMInfo_t g_PMM;

#define _1MB (1024 * 1024)
#define FE_BS sizeof(*g_PMM.buffer)

#define SET_PAGE(index) ({ \
    size_t pIndex = index / FE_BS; \
    size_t pBit = index % FE_BS; \
    \
    g_PMM.buffer[pIndex] |= (1 << pBit); \
})

#define CLEAR_PAGE(index) ({ \
    size_t pIndex = index / FE_BS; \
    size_t pBit = index % FE_BS; \
    \
    g_PMM.buffer[pIndex] &= ~(1 << pBit); \
})

#define TEST_BIT(num, b) ((num >> b) & 1)

static uint64_t getBlocks(const size_t blocks);

void PMM_Initialize(MemoryRegion_t *region)
{   
    g_PMM.start = region->start + _1MB;     // Skip the first 1MB
    g_PMM.blocks = region->len / BLOCK_SIZE;
    g_PMM.buffer = (uint32_t *)g_PMM.start; 
    g_PMM.total = g_PMM.free = region->len - _1MB;
    g_PMM.used = 0;
    
    // Mark all memory as unused
    LOGI("PMM", "Initializing Physical Memory...\n");
    memset((void *)g_PMM.buffer, 0, g_PMM.total);
    LOGI("PMM", "Physical Memory Initialized.\n");
}

uint64_t PMM_GetTotal()
{
    return g_PMM.total;
}

uint64_t PMM_GetFree()
{
    return g_PMM.free;
}

uint64_t PMM_GetUsed()
{
    return g_PMM.used;
}

void *PMM_AllocateBlock()
{
    return PMM_Allocate(1);
}

void *PMM_Allocate(const size_t blocks)
{
    if (blocks == 0)
        return NULL;
    
    // Find the beginning of this block    
    uint64_t index = getBlocks(blocks);
    for (size_t i = 0; i < blocks; i++)     // Reserve all blocks
        SET_PAGE(index + i);
    
    g_PMM.used += blocks * BLOCK_SIZE; g_PMM.free -= blocks * BLOCK_SIZE;
    return (void *)(g_PMM.start + index * BLOCK_SIZE);
}

bool PMM_FreeBlock(void *address)
{
    return PMM_Free(address, 1);
}

bool PMM_Free(void *address, const size_t blocks)
{
    // Check that the index divides by BLOCK_SIZE and that the block exists
    uint64_t index = (uint64_t)address - g_PMM.start;
    if (index % BLOCK_SIZE != 0 || index / BLOCK_SIZE > g_PMM.blocks)
        return false;
    
    for (size_t i = 0; i < blocks; i++)
        CLEAR_PAGE(index / BLOCK_SIZE + i);
    
    g_PMM.used -= blocks * BLOCK_SIZE; g_PMM.free += blocks * BLOCK_SIZE;
    return true;
}

uint64_t getBlocks(const size_t blocks)
{
    bool fbFound = false;
    uint64_t j, index = 0, bCopy = blocks;
    for (size_t i = 0; i < g_PMM.blocks; i++)
    {
        // Block is not completely used
        if (g_PMM.buffer[i] != 0xFFFFFFFF)
        {
            for (j = 0; j < FE_BS; j++)
            {
                if (!TEST_BIT(g_PMM.buffer[i], j))
                {
                    // If found the first block
                    if (!fbFound)
                    {
                        fbFound = true;
                        index = i * FE_BS + j;
                    }
                    
                    bCopy--;
                    if (bCopy == 0)
                        goto found;
                }
                else    // Block is occupied
                {
                    if (fbFound)    // Already found a couple of blocks
                    {
                        // Reset the data and continue looking
                        fbFound = false;
                        bCopy = blocks;
                    }
                }
            }
        }
    }
    
    KPANIC("Out of memory.\n");
    return 0;   // So the compiler doesn't cry
found:
    return index;
}