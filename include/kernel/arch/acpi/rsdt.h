#pragma once

#include <stdint.h>
#include <stdbool.h>

#define TABLE_LENGTH 4
#define ACPI_TABLE_NAME "FACP"

typedef struct RSDTHeader
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
} __attribute__((packed)) RSDTHeader_t;

typedef struct RSDTV1
{
    RSDTHeader_t header;
    uint32_t otherSDT[TABLE_LENGTH];
} __attribute__((packed)) RSDTV1_t;

typedef struct RSDTV2
{
    RSDTHeader_t header;
    uint64_t otherSDT[TABLE_LENGTH * 2];
} __attribute__((packed)) RSDTV2_t;

/**
 * @brief Validate an RSDT table.
 * 
 * @param header RSDT Header to validate.
 * @return True if successfully validated the table, False, otherwise.
*/
bool RSDT_Validate(const RSDTHeader_t *header);

/**
 * @brief Find a table in the old ACPI table.
 * 
 * @param root RSDT root.
 * @param tableName Name of the table to find.
 * @return Address of the found table, NULL otherwise.
*/
void *RSDT_FindTableV1(const RSDTV1_t *root, const char *tableName);

/**
 * @brief Find a table in the new ACPI table.
 * 
 * @param root RSDT root.
 * @param tableName Name of the table to find.
 * @return Address of the found table, NULL otherwise.
*/
void *RSDT_FindTableV2(const RSDTV2_t *root, const char *tableName);