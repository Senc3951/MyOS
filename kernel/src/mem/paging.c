#include <mem/paging.h>
#include <mem/pmm.h>
#include <panic.h>
#include <libc/string.h>
#include <log.h>

#define PTE_SET(entry, addr, f) { entry->flags = (f); entry->frame = ((addr) >> 12); }
#define PTE_FLAGS(entry)        (entry->flags)
#define PTE_ADDRESS(entry)      (entry->frame << 12)
#define PTE_PRESENT(entry)      ((bool)(PTE_FLAGS(entry) & PAGING_PRESENT))

#define PD_INDEX(x)             ((x >> 22) & 0x3ff)
#define PT_INDEX(x)             ((x >> 12) & 0x3ff)

static page_table_entry_t *walkpd(page_table_t *pd, uintptr_t vaddr, uint16_t flags, const bool alloc);
static void *intrn_virt2phys(uintptr_t virt, page_table_entry_t *ptentry);
static void intrn_unmap(uintptr_t virt, page_table_entry_t *ptentry);

page_table_t *pt_create()
{
    page_table_t *pt = (page_table_t *)pmm_getFrame();
    if (!pt)
        return NULL;
    
    memset(pt, 0, sizeof(page_table_t));
    return pt;
}

void pt_load(page_table_t *pd)
{
    uintptr_t addr = (uintptr_t)pd;

    extern void x32_pd_load(uintptr_t addr);
    x32_pd_load(addr);
}

void pt_map(page_table_t *pd, uintptr_t vaddr, uintptr_t paddr, uint16_t flags)
{
    flags |= PAGING_PRESENT;
    
    uintptr_t va = rndown(vaddr, PAGE_SIZE);
    page_table_entry_t *ptentry = walkpd(pd, va, flags, true);
    if (!ptentry)
        panic("page table entry null for virtual address %p", vaddr);
    
    PTE_SET(ptentry, paddr, flags);
}

void pt_mapv(page_table_t *pd, void *vaddr, void *paddr, uint16_t flags)
{
    pt_map(pd, (uintptr_t)vaddr, (uintptr_t)paddr, flags);
}

void pt_imap(page_table_t *pd, uintptr_t vaddr, uint16_t flags)
{
    pt_map(pd, vaddr, vaddr, flags);
}

void *pt_mapPage(page_table_t *pd, uintptr_t vaddr, uint16_t flags)
{
    void *frame = pmm_getFrame();
    if (!frame)
        return NULL;
    
    pt_map(pd, vaddr, (uintptr_t)frame, flags);
    return frame;
}

void pt_imapRange(page_table_t *pd, uintptr_t start, uintptr_t end, uint16_t flags)
{
    uintptr_t rstart = rndown(start, PAGE_SIZE);
    uintptr_t rend = rndup(end, PAGE_SIZE);
    for (uintptr_t addr = rstart; addr < rend; addr += PAGE_SIZE)
        pt_map(pd, addr, addr, flags);
}

void pt_imapvRange(page_table_t *pd, void *start, void *end, uint16_t flags)
{
    pt_imapRange(pd, (uintptr_t)start, (uintptr_t)end, flags);
}

void *pt_alloc_pages(page_table_t *pd, const size_t pc, uint16_t flags)
{
    if (!pc)
        return NULL;

    // Create first frame
    void *fb = pmm_getFrame();
    if (!fb)
        return NULL;
    
    // Map first frame
    uintptr_t firstFrame = (uintptr_t)fb;
    pt_map(pd, firstFrame, firstFrame, flags);
    
    // Map other frames
    for (size_t i = 1; i < pc; i++)
    {
        void *frame = pmm_getFrame();
        if (!frame)
            return NULL;
        
        uintptr_t virt = firstFrame + i * PAGE_SIZE;
        pt_map(pd, virt, (uintptr_t)frame, flags);
    }
    
    return fb;
}

void pt_free_pages(page_table_t *pd, void *ptr, const size_t pc)
{
    uintptr_t addr = (uintptr_t)ptr;
    for (size_t i = 0; i < pc; i++)
    {
        uintptr_t virt = addr + i * PAGE_SIZE;
        pt_unmap(pd, virt);
    }
}

void *virt2phys(page_table_t *pd, uintptr_t vaddr)
{
    uintptr_t va = rndown(vaddr, PAGE_SIZE);
    page_table_entry_t *ptentry = walkpd(pd, va, 0, false);
    
    return intrn_virt2phys(va, ptentry);
}

void pt_unmap(page_table_t *pd, uintptr_t vaddr)
{
    uintptr_t va = rndown(vaddr, PAGE_SIZE);
    page_table_entry_t *ptentry = walkpd(pd, va, 0, false);

    intrn_unmap(vaddr, ptentry);
}

page_table_entry_t *walkpd(page_table_t *pd, uintptr_t vaddr, uint16_t flags, const bool alloc)
{
    page_table_t *pt = NULL;
    page_table_entry_t *pde = &pd->entries[PD_INDEX(vaddr)];
    if (PTE_PRESENT(pde))
        pt = (page_table_t *)PTE_ADDRESS(pde);
    else
    {
        if (!alloc)
            return NULL;
        
        // Create new page table
        pt = pt_create();
        if (!pt)
            panic("failed creating page table");
        
        // Map page directory entry
        PTE_SET(pde, (uintptr_t)pt, flags);
    }
    
    return &pt->entries[PT_INDEX(vaddr)];
}

void *intrn_virt2phys(uintptr_t virt, page_table_entry_t *ptentry)
{
    // Entry doesn't exist or not present
    if (!ptentry || !PTE_PRESENT(ptentry))
        return NULL;
    
    return (void *)(PTE_ADDRESS(ptentry) | (virt & 0xFFF));
}

void intrn_unmap(uintptr_t virt, page_table_entry_t *ptentry)
{
    void *frame = intrn_virt2phys(virt, ptentry);
    if (!frame)
        return;
        
    PTE_SET(ptentry, 0, 0);     // Mark as not present
    pmm_releaseFrame(frame);    // Release physical frame
}