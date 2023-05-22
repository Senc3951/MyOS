#pragma once

#define PCI_CONFIG_PORT 0xCF8
#define PCI_DATA_PORT   0xCFC

#define PCI_INVALID_DEVICE 0xFFFF

void pci_init();