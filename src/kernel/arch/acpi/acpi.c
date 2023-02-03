#include <kernel/io/io.h>
#include <kernel/dev/pit.h>
#include <kernel/lib/string.h>
#include <kernel/sys/logger.h>
#include <kernel/arch/acpi/acpi.h>
#include <kernel/arch/acpi/rsdp.h>

ACPI_t *g_ACPI = NULL;
uint16_t g_SLP_TYPa;
uint16_t g_SLP_TYPb;
uint16_t g_SLP_EN;
uint16_t g_SCI_EN = 0;

static bool ACPI_Parse(const void *addr);

inline static bool validateACPIHeader(const uint32_t *ptr, const char *signature)
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

bool ACPI_ParseV1(struct multiboot_tag_old_acpi *tag)
{
    RSDPDescriptorV1_t *rsdp = (RSDPDescriptorV1_t *)tag->rsdp;
    if (!RSDP_ValidateV1(rsdp))
    {
        LOGE("ACPI", "RSDP.V1 Checksum is Invalid.\n");
        return false;
    }
    
    RSDTV1_t *rsdt = (RSDTV1_t *)rsdp->rsdtAddress;
    if (!RSDT_Validate(&rsdt->header))
    {
        LOGE("ACPI", "RSDT.V1 Checksum is Invalid.\n");
        return false;
    }
    
    void *ACPIAddr = RSDT_FindTableV1(rsdt, ACPI_TABLE_NAME);
    if (!ACPIAddr)
    {
        LOGE("ACPI", "RSDT.V1 Failed finding ACPI Table.\n");
        return false;
    }
    
    return ACPI_Parse(ACPIAddr);
}

bool ACPI_ParseV2(struct multiboot_tag_new_acpi *tag)
{
    RSDPDescriptorV2_t *rsdp = (RSDPDescriptorV2_t *)tag->rsdp;
    if (!RSDP_ValidateV2(rsdp))
    {
        LOGE("ACPI", "RSDP.V2 Checksum is Invalid.\n");
        return false;
    }
    
    RSDTV2_t *rsdt = (RSDTV2_t *)rsdp->xsdtAddress;
    if (!RSDT_Validate(&rsdt->header))
    {
        LOGE("ACPI", "RSDT.V2 Checksum is Invalid.\n");
        return false;
    }
    
    void *ACPIAddr = RSDT_FindTableV2(rsdt, ACPI_TABLE_NAME);
    if (!ACPIAddr)
    {
        LOGE("ACPI", "RSDT.V2 Failed finding ACPI Table.\n");
        return false;
    }
    
    return ACPI_Parse(ACPIAddr);
}

bool ACPI_Parse(const void *addr)
{
    g_ACPI = (ACPI_t *)addr;
    if (validateACPIHeader((uint32_t *)g_ACPI->dsdt, DSDT))
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
                LOGE("ACPI", "\\_S5 Parse error.\n");
        }
        else
            LOGE("ACPI", "\\_S5 Not present.\n");
    }
    else
        LOGE("ACPI", "Invalid DSDT.\n");
    
    return false;
}

bool ACPI_Enable()
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
                
                PIT_Sleep(10);
            }
            if (g_ACPI->PM1bControlBlock != 0)
            {
                for (; i < 300; i++)
                {
                    if ((inw((uint32_t)g_ACPI->PM1bControlBlock) & g_SCI_EN) == 1)
                        break;
                    
                    PIT_Sleep(10);
                }
            }
            
            if (i < 300)
                return true;
            else
                LOGE("ACPI", "Failed enabling ACPI.\n");
        }
        else
            LOGE("ACPI", "Failed enabling ACPI.\n");
    }
    else
        LOGW("ACPI", "ACPI already enabled.\n");
    
    return false;
}

void ACPI_Shutdown()
{
    if (g_SCI_EN == 0)
    {
        LOGE("ACPI", "ACPI is not enabled.\n");
        return;
    }
    
    outw((uint32_t)g_ACPI->PM1aControlBlock, g_SLP_TYPa | g_SLP_EN);
    if (g_ACPI->PM1bControlBlock != 0)
        outw((uint32_t)g_ACPI->PM1bControlBlock, g_SLP_TYPb | g_SLP_EN);
    
    LOGCE("ACPI", "Shutdown failed.\n");
}