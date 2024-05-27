#include <arch/rsdt.h>
#include <arch/cpu.h>
#include <boot/mb2.h>
#include <mem/vmm.h>
#include <log.h>
#include <libc/string.h>
#include <panic.h>
#include <assert.h>

#define RSDT_TABLE_SIZE 4

static rsdt_t *g_rsdt;
static size_t g_tableCount;
static bool g_old = true;

static inline bool validateTable(void *ptr, size_t count)
{
    uint8_t sum = 0;
    char *p = (char *)ptr;
    for (size_t i = 0; i < count; i++)
        sum += p[i];
       
    return sum == 0;
}

static void initRSDT(void *ptr)
{
    // Identity map rsdt
    uintptr_t rsdtRoundedAddress = rndown((uintptr_t)ptr, PAGE_SIZE);
    pt_imap(get_kernel_pd(), rsdtRoundedAddress, PAGING_READ_WRITE);
    __flush_tlb(rsdtRoundedAddress);
    
    // Validate rsdt
    g_rsdt = (rsdt_t *)ptr;
    assert(validateTable(ptr, g_rsdt->header.length));
    
    // Get table count
    rsdt_header_t *header = &g_rsdt->header;
    g_tableCount = (header->length - sizeof(rsdt_header_t)) / 4;
    if (!g_old)
        g_tableCount /= 2;
    
    // Iterate tables
    LOG_WARNING_INFO("RSDT at %p with %u tables: ", ptr, g_tableCount);
    for (size_t i = 0; i < g_tableCount; i++)
    {
        rsdt_header_t *tableHeader;
        if (g_old)
            tableHeader = (rsdt_header_t *)g_rsdt->tables[i];
        else
            tableHeader = (rsdt_header_t *)((xsdt_t *)ptr)->tables[i];
        
        // Validate table
        assert(validateTable(tableHeader, tableHeader->length));
        
        // Print table
        if (i == 0)
            LOG_WARNING_NO_INFO("%.4s", tableHeader->signature);
        else if (i < g_tableCount - 1)
            LOG_WARNING_NO_INFO(", %.4s", tableHeader->signature);
        else
            LOG_WARNING_NO_INFO(", %.4s\n", tableHeader->signature);
    }
}

void rsdp_init()
{
    // Get rsdp
    rsdp_t *rsdp = NULL;
    struct multiboot_tag_old_acpi *rsdp1 = (struct multiboot_tag_old_acpi *)mb2_get_tag(MULTIBOOT_TAG_TYPE_ACPI_OLD);
    if (rsdp1)
        rsdp = (rsdp_t *)rsdp1->rsdp;
    else
    {
        // RSDP2
        struct multiboot_tag_new_acpi *rsdp2 = (struct multiboot_tag_new_acpi *)mb2_get_tag(MULTIBOOT_TAG_TYPE_ACPI_NEW);
        if (!rsdp2)
            panic("rsdp not provided by multiboot");

        g_old = false;
        rsdp = (rsdp_t *)rsdp2->rsdp;
    }
    
    // Identity map rsdp
    uintptr_t rsdpRoundedAddress = rndown((uintptr_t)rsdp, PAGE_SIZE);
    pt_imap(get_kernel_pd(), rsdpRoundedAddress, PAGING_READ_WRITE);
    __flush_tlb(rsdpRoundedAddress);

    // Validate rsdp
    assert(validateTable(rsdp, sizeof(rsdp_t)));
    if (!g_old)
        assert(validateTable(&(((xsdp_t *)rsdp)->length), sizeof(xsdp_t) - sizeof(rsdp_t)));
    
    LOG("RSDP version %d at %p\n", g_old ? 1 : 2, (uintptr_t)rsdp);
    initRSDT(g_old ? (void *)rsdp->rsdtAddress : (void *)((xsdp_t *)rsdp)->xsdtAddress);
}

void *rsdt_find_table(const char *name)
{
    for (size_t i = 0; i < g_tableCount; i++)
    {
        rsdt_header_t *header;
        if (g_old)
            header = (rsdt_header_t *)g_rsdt->tables[i];
        else
            header = (rsdt_header_t *)((xsdt_t *)g_rsdt)->tables[i];
        
        if (!memcmp(header->signature, name, RSDT_TABLE_SIZE))
            return (void *)header;
    }
        
    panic("rsdt table %s not found\n", name);
}