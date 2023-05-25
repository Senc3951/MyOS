#include <acpi/rsdp.h>

bool rsdp_validate_old(const struct rsdp_descriptor_old *rsdp)
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

bool rsdp_validate_new(const struct rsdp_descriptor_new *rsdp)
{
    size_t res = rsdp->length;
    res += rsdp->xsdtAddress;
    res += rsdp->extendedChecksum;
    for (size_t i = 0; i < sizeof(rsdp->reserved); i++)
        res += rsdp->reserved[i];
    
    return ((res & 1) == 0) && rsdp_validate_old(&rsdp->firstPart);
}