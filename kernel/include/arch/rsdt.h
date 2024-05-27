#pragma once

#include <common.h>

typedef struct RSDP
{
    char signature[8];
    uint8_t checksum;
    char _OEMID[6];
    uint8_t revision;
    uint32_t rsdtAddress;
} __PACKED__ rsdp_t;

typedef struct XSDP
{
    rsdp_t rsdp;
    uint32_t length;
    uint64_t xsdtAddress;
    uint8_t extendedChecksum;
    uint8_t reserved[3];
} __PACKED__ xsdp_t;

typedef struct RSDT_HEADER
{
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char _OEMID[6];
    char _OEMTableID[8];
    uint32_t _OEMRevision;
    uint32_t creatorID;
    uint32_t creatorRevision;
} __PACKED__ rsdt_header_t;

typedef struct RSDT
{
    rsdt_header_t header;
    uint32_t tables[]; 
} __PACKED__ rsdt_t;

typedef struct XSDT
{
    rsdt_header_t header;
    uint64_t tables[]; 
} __PACKED__ xsdt_t;

#define RSDT_NAME_APIC  "APIC"
#define RSDT_NAME_ACPI  "FACP"

void rsdp_init();
void *rsdt_find_table(const char *name);