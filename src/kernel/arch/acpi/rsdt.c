#include <kernel/lib/string.h>
#include <kernel/arch/acpi/rsdt.h>
#include <kernel/dev/display/vga.h>
bool RSDT_Validate(const RSDTHeader_t *header)
{
    uint8_t sum = 0;
    for (int i = 0; i < header->length; i++)
        sum += ((char *)header)[i];
    
    return sum == 0;
}

void *RSDT_FindTableV1(const RSDTV1_t *root, const char *tableName)
{
    size_t entries = (root->header.length - sizeof(root->header)) / 4;
    for (int i = 0; i < entries; i++)
    {
        RSDTHeader_t *header = (RSDTHeader_t *)root->otherSDT[i];
        
        if (strncmp(header->signature, tableName, TABLE_LENGTH) == 0)
            return (void *)header;
    }
    
    return NULL;
}

void *RSDT_FindTableV2(const RSDTV2_t *root, const char *tableName)
{
    size_t entries = (root->header.length - sizeof(root->header)) / 8;
    for (int i = 0; i < entries; i++)
    {
        RSDTHeader_t *header = (RSDTHeader_t *)root->otherSDT[i];
        if (strncmp(header->signature, tableName, TABLE_LENGTH) == 0)
            return (void *)header;
    }
    
    return NULL;
}