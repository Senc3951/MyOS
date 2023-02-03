#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct RSDPDescriptorV1
{
    char signature[8];
    uint8_t checksum;
    char OEMID[6];
    uint8_t revision;
    uint32_t rsdtAddress;
} __attribute__((packed)) RSDPDescriptorV1_t;

typedef struct RSDPDescriptorV2
{
    RSDPDescriptorV1_t firstPart;
    
    uint32_t length;
    uint64_t xsdtAddress;
    uint8_t extendedChecksum;
    uint8_t reserved[3];
} __attribute__ ((packed)) RSDPDescriptorV2_t;

/**
 * @brief Validate an RSDP V1 table.
 * 
 * @param rsdp RSDP Header to validate.
 * @return True if successfully validated the table, False, otherwise.
*/
bool RSDP_ValidateV1(const RSDPDescriptorV1_t *rsdp);

/**
 * @brief Validate an RSDP V2 table.
 * 
 * @param rsdp RSDP Header to validate.
 * @return True if successfully validated the table, False, otherwise.
*/
bool RSDP_ValidateV2(const RSDPDescriptorV2_t *rsdp);