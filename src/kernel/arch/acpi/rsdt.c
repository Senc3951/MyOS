#include <string.h>
#include <arch/acpi/rsdt.h>

bool rsdt_validate(const struct rsdt_header *header)
{
    uint8_t sum = 0;
    for (int i = 0; i < header->length; i++)
        sum += ((char *)header)[i];
    
    return sum == 0;
}

void *rsdt_find_table_old(const struct rsdt_old *root, const char *tableName)
{
    size_t entries = (root->header.length - sizeof(root->header)) / 4;
    for (int i = 0; i < entries; i++)
    {
        struct rsdt_header *header = (struct rsdt_header *)root->otherSDT[i];
        if (strncmp(header->signature, tableName, TABLE_LENGTH) == 0)
            return (void *)header;
    }
    
    return NULL;
}

void *rsdt_find_table_new(const struct rsdt_new *root, const char *tableName)
{
    size_t entries = (root->header.length - sizeof(root->header)) / 8;
    for (int i = 0; i < entries; i++)
    {
        struct rsdt_header *header = (struct rsdt_header *)root->otherSDT[i];
        if (strncmp(header->signature, tableName, TABLE_LENGTH) == 0)
            return (void *)header;
    }
    
    return NULL;
}