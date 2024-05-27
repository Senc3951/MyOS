#include <mem/vmm.h>
#include <mem/pmm.h>
#include <arch/idt.h>
#include <arch/cpu.h>
#include <assert.h>
#include <panic.h>
#include <log.h>

#define PAGE_FAULT_NUM  0xE
#define PAT_MSR         0x277

enum
{
    PF_ERROR_PRESENT = 1 << 0,
    PF_ERROR_WRITE   = 1 << 1,
    PF_ERROR_USER    = 1 << 2
};

static page_table_t *g_kernelPD = NULL;

static void initPAT()
{
    // Validate PAT is available
    uint32_t unused, edx;
    __cpuid(1, unused, unused, unused, edx);
    assert(edx & CPU_PAT);
    
    // WC at index 1
    uint64_t pat = __rdmsrb(PAT_MSR);
    pat = (pat & 0xFFFFFFFFFFFF00FF) | 0x0100;
    __wrmsrb(PAT_MSR, pat);

    uint64_t ret = __rdmsrb(PAT_MSR);
    assert(ret == pat);
    LOG_WARNING_INFO("PAT is %llx\n", ret);
}

static void pageFaultHandler(interrupt_frame_t *intFrame)
{
    uintptr_t faultAddress = __readcr2();
    if (!faultAddress)
        panic("page fault at address 0");
    if (intFrame->errorcode & PF_ERROR_PRESENT)
        panic("page fault at address %p because of protection violation", faultAddress);
    
    void *frame = pt_mapPage(g_kernelPD, faultAddress, PAGING_READ_WRITE);
    if (!frame)
        panic("failed mapping virtual address %p with error code %x", faultAddress, intFrame->errorcode);
    
    __flush_tlb(faultAddress);
    LOG("Mapped virtual address %p to physical address %p\n", faultAddress, frame);
}

void vmm_init()
{
    // Check pat exists and initialize
    initPAT();
    
    // Register page fault interrupt handler
    register_interrupt(PAGE_FAULT_NUM, pageFaultHandler);

    // Create page directory
    g_kernelPD = pt_create();
    if (!g_kernelPD)
        panic("failed creating kernel page directory");

    // Map memory
    pt_imapRange(g_kernelPD, 0, pmm_memend(), PAGING_READ_WRITE);

    // Map protected kernel region
    pt_imapRange(g_kernelPD, _KERNEL_WRITE_PROTECT_START, _KERNEL_WRITE_PROTECT_END, 0);
    
    // Load the page directory
    pt_load(g_kernelPD);
    
    // Enable paging
    extern void x32_paging_enable();
    x32_paging_enable();
    
    LOG_WARNING_INFO("Paging enabled with kernel page directory at %p\n", (uintptr_t)g_kernelPD);
}

page_table_t *get_kernel_pd() { return g_kernelPD; }