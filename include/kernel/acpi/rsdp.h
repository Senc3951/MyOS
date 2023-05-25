#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct rsdp_descriptor_old
{
    char signature[8];
    uint8_t checksum;
    char OEMID[6];
    uint8_t revision;
    uint32_t rsdtAddress;
} __attribute__((packed));

struct rsdp_descriptor_new
{
    struct rsdp_descriptor_old firstPart;
    
    uint32_t length;
    uint64_t xsdtAddress;
    uint8_t extendedChecksum;
    uint8_t reserved[3];
} __attribute__ ((packed));

/**
 * @brief Validate an RSDP V1 table.
 * 
 * @param rsdp RSDP Header to validate.
 * @return True if successfully validated the table, False, otherwise.
*/
bool rsdp_validate_old(const struct rsdp_descriptor_old *rsdp);

/**
 * @brief Validate an RSDP V2 table.
 * 
 * @param rsdp RSDP Header to validate.
 * @return True if successfully validated the table, False, otherwise.
*/
bool rsdp_validate_new(const struct rsdp_descriptor_new *rsdp);