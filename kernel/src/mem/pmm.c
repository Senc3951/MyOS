#include <mem/pmm.h>
#include <boot/mb2.h>
#include <arch/spinlock.h>
#include <panic.h>
#include <assert.h>
#include <libc/string.h>
#include <log.h>

static uint64_t *g_bitmap = NULL;
static uintptr_t g_memend = 0, g_lastEntryFound = 0;
static uint32_t g_elementSizeInBits, g_bitmapElements = 0;
static spinlock_t g_lock;

#define MAX_BITMAP_VALUE            UINT64_MAX
#define BITMAP_CLEAR_FRAME(i, j)    (g_bitmap[i] &= ~(1ULL << j))

static void freeRegion(uintptr_t start, size_t size);
static void reserveRegion(uintptr_t start, size_t size);
static uintptr_t findFrame();

static inline void setFrame(const uintptr_t index)
{
    uintptr_t i = index / g_elementSizeInBits;
    uintptr_t j = index % g_elementSizeInBits;

    g_bitmap[i] |= (1ULL << j);
}

static inline void clearFrame(const uintptr_t index)
{
    uintptr_t i = index / g_elementSizeInBits;
    uintptr_t j = index % g_elementSizeInBits;

    BITMAP_CLEAR_FRAME(i, j);
}

static inline bool testFrame(const uintptr_t i, const uint8_t bit)
{
    return g_bitmap[i] & (1ULL << bit);
}

void pmm_init()
{
    g_elementSizeInBits = sizeof(*g_bitmap) * 8;
    spinlock_init(&g_lock);
    
    // Get memory map
    const struct multiboot_tag_mmap *mmap = mb2_get_tag(MULTIBOOT_TAG_TYPE_MMAP);
    
    // Find highest available address
    multiboot_memory_map_t *kentry = NULL;
    uint32_t entries = mmap->size / mmap->entry_size;
    for (uint32_t i = 0; i < entries; i++)
    {
        multiboot_memory_map_t *entry = (multiboot_memory_map_t *)((uintptr_t)mmap->entries + i * mmap->entry_size);
        LOG("Physical region at %llx-%llx [%u]\n", entry->addr, entry->addr + entry->len, entry->type);

        // Find top physical memory
        if (entry->type == MULTIBOOT_MEMORY_AVAILABLE && entry->addr + entry->len > g_memend)
        {
            if (_KERNEL_START >= entry->addr && _KERNEL_END <= entry->addr + entry->len)
                kentry = entry;
            
            g_memend = entry->addr + entry->len;
        }
    }
    
    g_bitmapElements = g_memend / FRAME_SIZE / g_elementSizeInBits;
    
    // Find location to store the bitmap
    uint32_t roundedKernelEnd = rndup(_KERNEL_END, FRAME_SIZE);
    size_t bitmapSize = g_bitmapElements * sizeof(*g_bitmap);
    
    // Can store with the kernel segment
    if (roundedKernelEnd + bitmapSize <= kentry->addr + kentry->len)
        g_bitmap = (uint64_t *)roundedKernelEnd;
    else
    {
        // Search for another segment
        for (size_t i = 0; i < entries; i++)
        {
            multiboot_memory_map_t *entry = (multiboot_memory_map_t *)((uintptr_t)mmap->entries + i * mmap->entry_size);
            if (entry->type == MULTIBOOT_MEMORY_AVAILABLE && entry != kentry && entry->len >= bitmapSize)
            {
                g_bitmap = (uint64_t *)entry->addr;
                break;
            }
        }
    }
    
    if (!g_bitmap)
        panic("failed finding address to store the bitmap at");
    
    // Reserve all memory
    memset(g_bitmap, 0xFF, bitmapSize);
    
    // Free available regions
    for (size_t i = 0; i < entries; i++)
    {
        multiboot_memory_map_t *entry = (multiboot_memory_map_t *)((uintptr_t)mmap->entries + i * mmap->entry_size);
        if (entry->type == MULTIBOOT_MEMORY_AVAILABLE)
            freeRegion(entry->addr, entry->len);
    }

    // Reserve kernel
    reserveRegion(_KERNEL_START, roundedKernelEnd - _KERNEL_START);
    
    // Reserve bitmap
    reserveRegion((uint64_t)g_bitmap, bitmapSize);
    
    // Reserve lower 2MiB
    reserveRegion(0, 2 * _MB);
    
    LOG_WARNING_INFO("%u-bit bitmap at %p with %u elements\n", g_elementSizeInBits, (uintptr_t)g_bitmap, g_bitmapElements);
}

void *pmm_getFrame()
{
    spinlock_acquire(&g_lock);

    uintptr_t index = findFrame();
    setFrame(index);
    
    spinlock_release(&g_lock);
    return (void *)(index * FRAME_SIZE);
}

void pmm_releaseFrame(void *ptr)
{
    spinlock_acquire(&g_lock);

    // Check for valid address
    uintptr_t addr = (uintptr_t)ptr;
    if (addr % FRAME_SIZE != 0 || addr / FRAME_SIZE > g_bitmapElements)
        panic("invalid physical address %p", addr);
    
    // Check the frame is marked as used
    uintptr_t index = addr / FRAME_SIZE;
    uintptr_t i = index / g_elementSizeInBits;
    uintptr_t j = index % g_elementSizeInBits;
    if (!testFrame(i, j))
        panic("frame %p is not used", addr);
    
    g_lastEntryFound = i;
    BITMAP_CLEAR_FRAME(i, j);
    
    spinlock_release(&g_lock);
}

uint64_t pmm_memend()
{
    return g_memend;
}

uintptr_t findFrame()
{
    // Search from last entry found
    for (uintptr_t i = g_lastEntryFound; i < g_bitmapElements; i++)
    {
        if (g_bitmap[i] == MAX_BITMAP_VALUE)
            continue;
        
        // Search each bit
        for (uintptr_t j = 0; j < g_elementSizeInBits; j++)
        {
            if (!testFrame(i, j))   // Found available frame
            {
                g_lastEntryFound = i;
                return i * g_elementSizeInBits + j;
            }
        }
    }
    
    // Search again from the start
    if (g_lastEntryFound)
    {
        g_lastEntryFound = 0;
        return findFrame();
    }
    
    panic("out of memory");
    return 0;
}

void freeRegion(uintptr_t start, size_t size)
{
    size_t fc = rndown(size, FRAME_SIZE) / FRAME_SIZE;
    for (size_t i = 0; i < fc; i++)
        clearFrame((start + i * FRAME_SIZE) / FRAME_SIZE);
}

void reserveRegion(uintptr_t start, size_t size)
{
    size_t fc = rndup(size, FRAME_SIZE) / FRAME_SIZE;
    for (size_t i = 0; i < fc; i++)
        setFrame((start + i * FRAME_SIZE) / FRAME_SIZE);
}