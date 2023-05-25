#pragma once

#include <stdint.h>
#include <stdbool.h>

#define TABLE_LENGTH 4
#define ACPI_TABLE_NAME "FACP"

struct rsdt_header
{
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t creatorID;
    uint32_t creatorRevision;
} __attribute__((packed));

struct rsdt_old
{
    struct rsdt_header header;
    uint32_t otherSDT[TABLE_LENGTH];
} __attribute__((packed));

struct rsdt_new
{
    struct rsdt_header header;
    uint64_t otherSDT[TABLE_LENGTH * 2];
} __attribute__((packed));

/**
 * @brief Validate an RSDT table.
 * 
 * @param header RSDT Header to validate.
 * @return True if successfully validated the table, False, otherwise.
*/
bool rsdt_validate(const struct rsdt_header *header);

/**
 * @brief Find a table in the old ACPI table.
 * 
 * @param root RSDT root.
 * @param tableName Name of the table to find.
 * @return Address of the found table, NULL otherwise.
*/
void *rsdt_find_table_old(const struct rsdt_old *root, const char *tableName);

/**
 * @brief Find a table in the new ACPI table.
 * 
 * @param root RSDT root.
 * @param tableName Name of the table to find.
 * @return Address of the found table, NULL otherwise.
*/
void *rsdt_find_table_new(const struct rsdt_new *root, const char *tableName);