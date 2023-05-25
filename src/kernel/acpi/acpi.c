#include <io/io.h>
#include <string.h>
#include <dev/pit.h>
#include <acpi/acpi.h>
#include <acpi/rsdp.h>
#include <sys/panic.h>
#include <sys/logger.h>

struct ACPI *g_ACPI = NULL;
uint16_t g_SLP_TYPa, g_SLP_TYPb, g_SLP_EN, g_SCI_EN = 0;

inline static bool validate_acpi_header(const uint32_t *ptr, const char *signature)
{
    if (memcmp(ptr, signature, 4) == 0)
    {
        char *checkPtr = (char *)ptr;
        int len = *(ptr + 1);
        char check = 0;

        while (0 < len--)
        {
            check += *checkPtr;
            checkPtr++;
        }
        
        if (check == 0)
            return true;
    }

    return false;
}

static bool acpi_parse(const void *addr)
{
    g_ACPI = (struct ACPI *)addr;
    if (validate_acpi_header((uint32_t *)g_ACPI->dsdt, DSDT))
    {
        char *_S5Addr = (char *)g_ACPI->dsdt + 36;   // Skip header
        int dsdtLength = (g_ACPI->dsdt + 1) - 36;
        
        while (0 < dsdtLength--)
        {
            if (strncmp(_S5Addr, __S5, 4) == 0)
                break;
            
            _S5Addr++;
        }
        
        // Check if \_S5 was found
        if (dsdtLength > 0)
        {
            // Check for valid AML structure
            if ((*(_S5Addr - 1) == 0x08 || (*(_S5Addr - 2) == 0x08 && *(_S5Addr - 1) == '\\')) && *(_S5Addr + 4) == 0x12)
            {
                _S5Addr += 5;
                _S5Addr += ((*_S5Addr &0xC0) >> 6) + 2;   // Calculate PkgLength size
                
                if (*_S5Addr == 0x0A)
                    _S5Addr++;   // Skip byte prefix
                
                g_SLP_TYPa = *(_S5Addr) << 10;
                _S5Addr++;

                if (*_S5Addr == 0x0A)
                    _S5Addr++;   // Skip byte prefix
                
                g_SLP_TYPb = *(_S5Addr) << 10;
                g_SLP_EN = 1 << 13;
                g_SCI_EN = 1;

                return true;
            }
            else
                LOG("\\_S5 Parse error.\n");
        }
        else
            LOG("\\_S5 Not present.\n");
    }
    else
        LOG("Invalid DSDT.\n");
    
    return false;
}

bool acpi_parse_old(struct multiboot_tag_old_acpi *tag)
{
    struct rsdp_descriptor_old *rsdp = (struct rsdp_descriptor_old *)tag->rsdp;
    if (!rsdp_validate_old(rsdp))
    {
        LOG("RSDP.V1 Checksum is Invalid.\n");
        return false;
    }
    
    struct rsdt_old *rsdt = (struct rsdt_old *)rsdp->rsdtAddress;
    if (!rsdt_validate(&rsdt->header))
    {
        LOG("RSDT.V1 Checksum is Invalid.\n");
        return false;
    }
    
    void *acpiAddr = rsdt_find_table_old(rsdt, ACPI_TABLE_NAME);
    if (!acpiAddr)
    {
        LOG("RSDT.V1 Failed finding ACPI Table.\n");
        return false;
    }
    
    return acpi_parse(acpiAddr);
}

bool acpi_parse_new(struct multiboot_tag_new_acpi *tag)
{
    struct rsdp_descriptor_new *rsdp = (struct rsdp_descriptor_new *)tag->rsdp;
    if (!rsdp_validate_new(rsdp))
    {
        LOG("RSDP.V2 Checksum is Invalid.\n");
        return false;
    }
    
    struct rsdt_new *rsdt = (struct rsdt_new *)rsdp->xsdtAddress;
    if (!rsdt_validate(&rsdt->header))
    {
        LOG("RSDT.V2 Checksum is Invalid.\n");
        return false;
    }
    
    void *acpiAddr = rsdt_find_table_new(rsdt, ACPI_TABLE_NAME);
    if (!acpiAddr)
    {
        LOG("RSDT.V2 Failed finding ACPI Table.\n");
        return false;
    }
    
    return acpi_parse(acpiAddr);
}

bool acpi_enable()
{
    // Check if ACPI already enabled.
    if ((inw((uint32_t)g_ACPI->PM1aControlBlock) & g_SCI_EN) == 0)
    {
        // Check if ACPI can be enabled
        if (g_ACPI->SMI_CommandPort != 0 && g_ACPI->acpiEnable != 0)
        {
            outb((uint32_t)g_ACPI->SMI_CommandPort, g_ACPI->acpiEnable);   // Enable ACPI
            
            // Wait for hardware to change modes
            size_t i = 0;
            for (i = 0; i < 300; i++)
            {
                if ((inw((uint32_t)g_ACPI->PM1aControlBlock) & g_SCI_EN) == 1)
                    break;
                
                pit_sleep(10);
            }
            if (g_ACPI->PM1bControlBlock != 0)
            {
                for (; i < 300; i++)
                {
                    if ((inw((uint32_t)g_ACPI->PM1bControlBlock) & g_SCI_EN) == 1)
                        break;
                    
                    pit_sleep(10);
                }
            }
            
            if (i < 300)
                return true;
            else
                LOG("Failed enabling ACPI.\n");
        }
        else
            LOG("Failed enabling ACPI.\n");
    }
    else
        LOG("ACPI already enabled.\n");
    
    return false;
}

void acpi_shutdown()
{
    if (g_SCI_EN == 0)
    {
        LOG("ACPI is not enabled.\n");
        return;
    }
    
    outw((uint32_t)g_ACPI->PM1aControlBlock, g_SLP_TYPa | g_SLP_EN);
    if (g_ACPI->PM1bControlBlock != 0)
        outw((uint32_t)g_ACPI->PM1bControlBlock, g_SLP_TYPb | g_SLP_EN);
    
    KPANIC("Shutdown failed\n");
}