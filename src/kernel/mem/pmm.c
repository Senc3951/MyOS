#include <mem/pmm.h>
#include <sys/panic.h>
#include <lib/string.h>
#include <sys/logger.h>

struct pmm_info g_PMM;    // Holds the actual memory
uint64_t g_Total;
uint64_t g_FreeMemory;
uint64_t g_UsedMemory;
uint32_t *g_Buffer; // Holds the state of each page

#define _1MB (1024 * 1024)
#define FE_BS sizeof(*g_PMM.buffer)

#define SET_PAGE(index) ({ \
    size_t pIndex = index / FE_BS; \
    size_t pBit = index % FE_BS; \
    \
    g_Buffer[pIndex] |= (1 << pBit); \
})
#define CLEAR_PAGE(index) ({ \
    size_t pIndex = index / FE_BS; \
    size_t pBit = index % FE_BS; \
    \
    g_Buffer[pIndex] &= ~(1 << pBit); \
})
#define TEST_BIT(num, b) ((num >> b) & 1)

static uint64_t getBlocks(const size_t blocks);

void pmm_init(struct memory_region *region)
{   
    // 32 blocks of 4096 bytes is one chunk.
    uint64_t chunks = region->len / PAGE_SIZE / FE_BS;
    
    // The states are stored at the beginning, after them comes the actual memory.
    // While this is not the best solution, it's easy and only takes ~16KB for 2GB of memory.
    
    g_Buffer = (uint32_t *)(region->start + _1MB);    // Reserve the first 1MB
    g_PMM.start = (uint64_t)g_Buffer + chunks;    // Start after the buffer
    g_PMM.blocks = chunks;
    g_PMM.buffer = (uint32_t *)g_PMM.start; 
        
    g_Total = g_FreeMemory = region->len - _1MB; g_UsedMemory = 0;
    LOG("Chunks: 0x%x, State buffer start: 0x%x\n", chunks, g_Buffer);
    
    // Mark all memory as unused (state buffer and memory buffer)
    memset((void *)g_Buffer, 0, chunks);
    LOG("Physical memory manager Initialized.\n");
}

uint64_t pmm_get_total()
{
    return g_Total;
}

uint64_t pmm_get_free()
{
    return g_FreeMemory;
}

uint64_t pmm_get_used()
{
    return g_UsedMemory;
}

void *pmm_alloc(const size_t blocks)
{
    if (blocks == 0)
        return NULL;
    
    // Find the beginning of this block    
    uint64_t index = getBlocks(blocks);
    for (size_t i = 0; i < blocks; i++)     // Reserve all blocks
        SET_PAGE(index + i);
    
    g_UsedMemory += blocks * PAGE_SIZE; g_FreeMemory -= blocks * PAGE_SIZE;
    return (void *)(g_PMM.start + index * PAGE_SIZE);
}

bool pmm_free(void *address, const size_t blocks)
{
    // Check that the index divides by PAGE_SIZE and that the block exists
    uint64_t index = (uint64_t)address - g_PMM.start;
    if (index % PAGE_SIZE != 0 || index / PAGE_SIZE > g_PMM.blocks)
        return false;
    
    for (size_t i = 0; i < blocks; i++)
        CLEAR_PAGE(index / PAGE_SIZE + i);
    
    g_UsedMemory -= blocks * PAGE_SIZE; g_FreeMemory += blocks * PAGE_SIZE;
    return true;
}

uint64_t getBlocks(const size_t blocks)
{
    bool fbFound = false;
    uint64_t j, index = 0, bCopy = blocks;
    for (size_t i = 0; i < g_PMM.blocks; i++)
    {
        // Block is not completely used
        if (g_Buffer[i] != 0xFFFFFFFF)
        {
            for (j = 0; j < FE_BS; j++)
            {
                if (!TEST_BIT(g_Buffer[i], j))
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
    return 0;   // To avoid compiler warnings
found:
    return index;
}