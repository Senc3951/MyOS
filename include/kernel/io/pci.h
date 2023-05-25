#pragma once

#define PCI_CONFIG_PORT 0xCF8
#define PCI_DATA_PORT   0xCFC

#define PCI_MAX_SLOTS       32
#define PCI_MAX_FUNCTIONS   8
#define PCI_INVALID_DEVICE  0xFFFF

#define PCI_VENDOR_OFFSET   0x0
#define PCI_DEVICE_OFFSET   0x2
#define PCI_CLASS_OFFSET    0xA
#define PCI_HEADER_OFFSET   0xE

void pci_init();