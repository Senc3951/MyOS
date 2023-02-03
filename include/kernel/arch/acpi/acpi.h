#pragma once

#include "rsdt.h"
#include <kernel/boot/mb2.h>

#define DSDT "DSDT"
#define __S5 "_S5_"

typedef struct GenericAddressStructure
{
    uint8_t addressSpace;
    uint8_t bitWidth;
    uint8_t bitOffset;
    uint8_t accessSize;
    uint64_t address;
} __attribute__((packed)) GenericAddressStructure_t;

typedef struct ACPI
{
    RSDTHeader_t header;
    uint32_t firmwareCtrl;
    uint32_t dsdt;
 
    // field used in ACPI 1.0; no longer in use, for compatibility only
    uint8_t  reserved;
 
    uint8_t  preferredPowerManagementProfile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CommandPort;
    uint8_t  acpiEnable;
    uint8_t  acpiDisable;
    uint8_t  S4BIOS_REQ;
    uint8_t  PSTATE_Control;
    uint32_t PM1aEventBlock;
    uint32_t PM1bEventBlock;
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
    uint32_t PM2ControlBlock;
    uint32_t PMTimerBlock;
    uint32_t GPE0Block;
    uint32_t GPE1Block;
    uint8_t  PM1EventLength;
    uint8_t  PM1ControlLength;
    uint8_t  PM2ControlLength;
    uint8_t  PMTimerLength;
    uint8_t  GPE0Length;
    uint8_t  GPE1Length;
    uint8_t  GPE1Base;
    uint8_t  CStateControl;
    uint16_t worstC2Latency;
    uint16_t worstC3Latency;
    uint16_t flushSize;
    uint16_t flushStride;
    uint8_t  dutyOffset;
    uint8_t  dutyWidth;
    uint8_t  dayAlarm;
    uint8_t  monthAlarm;
    uint8_t  century;
 
    // reserved in ACPI 1.0; used since ACPI 2.0+
    uint16_t bootArchitectureFlags;
 
    uint8_t reserved2;
    uint32_t flags;
    
    // 12 byte structure; see below for details
    GenericAddressStructure_t ResetReg;
 
    uint8_t resetValue;
    uint8_t reserved3[3];
 
    // 64bit pointers - Available on ACPI 2.0+
    uint64_t X_FirmwareControl;
    uint64_t X_Dsdt;
    
    GenericAddressStructure_t X_PM1aEventBlock;
    GenericAddressStructure_t X_PM1bEventBlock;
    GenericAddressStructure_t X_PM1aControlBlock;
    GenericAddressStructure_t X_PM1bControlBlock;
    GenericAddressStructure_t X_PM2ControlBlock;
    GenericAddressStructure_t X_PMTimerBlock;
    GenericAddressStructure_t X_GPE0Block;
    GenericAddressStructure_t X_GPE1Block;
} __attribute__((packed)) ACPI_t;

/**
 * @brief Find and Parse the ACPI V1 table.
 * 
 * @param tag Information about the RSDP.
 * @return True if successfully parsed the table, False, otherwise.
*/
bool ACPI_ParseV1(struct multiboot_tag_old_acpi *tag);

/**
 * @brief Find and Parse the ACPI V2 table.
 * 
 * @param tag Information about the RSDP.
 * @return True if successfully parsed the table, False, otherwise.
*/
bool ACPI_ParseV2(struct multiboot_tag_new_acpi *tag);

/**
 * @brief Enable ACPI.
 * 
 * @return True if successfully enabled ACPI, False, otherwise.
*/
bool ACPI_Enable();

/**
 * @brief Shutdown using ACPI.
*/
void ACPI_Shutdown();