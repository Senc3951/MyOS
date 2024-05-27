#pragma once

#include <common.h>

#define PAGE_SIZE               4096
#define PAGE_TABLE_ENTRY_COUNT  1024

enum
{
    PAGING_PRESENT         = 0x1,
    PAGING_READ_WRITE      = 0x2,
    PAGING_USER_ACCESIBLE  = 0x4,
    PAGING_WRITE_THROUGH   = 0x8,
    PAGING_CACHE_DISABLE   = 0x10,
    PAGING_ACCESSED        = 0x20,
    PAGING_DIRTY           = 0x40,
    PAGING_PAT             = 0x80,
    PAGING_GLOBAL          = 0x100
};

#define PAGING_CACHE_WC         (PAGING_WRITE_THROUGH)

typedef struct PAGE_TABLE_ENTRY
{
    uint32_t flags : 12;
    uint32_t frame : 20;
} __PACKED__ page_table_entry_t;

typedef struct PAGE_TABLE
{
    page_table_entry_t entries[PAGE_TABLE_ENTRY_COUNT];
} __PACKED__ page_table_t;

page_table_t *pt_create();
void pt_load(page_table_t *pd);

void pt_map(page_table_t *pd, uintptr_t vaddr, uintptr_t paddr, uint16_t flags);
void pt_mapv(page_table_t *pd, void *vaddr, void *paddr, uint16_t flags);
void pt_imap(page_table_t *pd, uintptr_t vaddr, uint16_t flags);
void *pt_mapPage(page_table_t *pd, uintptr_t vaddr, uint16_t flags);
void pt_imapRange(page_table_t *pd, uintptr_t start, uintptr_t end, uint16_t flags);
void pt_imapvRange(page_table_t *pd, void *start, void *end, uint16_t flags);

void *pt_alloc_pages(page_table_t *pd, const size_t pc, uint16_t flags);
void pt_free_pages(page_table_t *pd, void *ptr, size_t pc);

void *virt2phys(page_table_t *pd, uintptr_t vaddr);
void pt_unmap(page_table_t *pd, uintptr_t vaddr);