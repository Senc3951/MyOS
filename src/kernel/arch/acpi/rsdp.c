#include <kernel/arch/acpi/rsdp.h>

bool RSDP_ValidateV1(const RSDPDescriptorV1_t *rsdp)
{
    size_t res = rsdp->rsdtAddress;
    res += rsdp->revision;
    res += rsdp->checksum;
    
    for (size_t i = 0; i < sizeof(rsdp->signature); i++)
        res += rsdp->signature[i];
    for (size_t i = 0; i < sizeof(rsdp->OEMID); i++)
        res += rsdp->OEMID[i];
    
    return (res & 1) == 0;
}

bool RSDP_ValidateV2(const RSDPDescriptorV2_t *rsdp)
{
    size_t res = rsdp->length;
    res += rsdp->xsdtAddress;
    res += rsdp->extendedChecksum;
    for (size_t i = 0; i < sizeof(rsdp->reserved); i++)
        res += rsdp->reserved[i];
    
    return ((res & 1) == 0) && RSDP_ValidateV1(&rsdp->firstPart);
}