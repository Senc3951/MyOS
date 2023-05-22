#include <io/io.h>
#include <dev/pci.h>
#include <stdbool.h>
#include <sys/logger.h>

static void check_device(uint8_t bus, uint8_t device);
static void check_function(uint8_t bus, uint8_t slot, uint8_t function);

static inline uint16_t read_config_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    
    // Create configuration address as per Figure 1
    uint32_t address = (uint32_t)((lbus << 16) | (lslot << 11) |
                (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    // Write out the address
    outl(PCI_CONFIG_PORT, address);
    
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    return (uint16_t)((inl(PCI_DATA_PORT) >> ((offset & 2) * 8)) & 0xFFFF);
}

#define get_vendor_id(bus, slot, func) (read_config_word(bus, slot, func, 0))
#define get_device_id(bus, slot, func) (read_config_word(bus, slot, func, 2))
#define get_header_type(bus, slot, func) ((read_config_word(bus, slot, func, 0xE) & 0x00FF))
#define get_base_class(bus, slot, func) ((read_config_word(bus, slot, func, 0xA) >> 8))
#define get_subclass(bus, slot, func) ((read_config_word(bus, slot, func, 0xA) & 0x00FF))

#define check_bus(bus) ({ \
    for (uint8_t device = 0; device < 32; device++) \
        check_device(bus, device); \
})

void check_device(uint8_t bus, uint8_t device)
{    
    uint16_t vendorID = get_vendor_id(bus, device, 0);
    if (vendorID == PCI_INVALID_DEVICE)
        return;
    
    LOG("Found s-function device. Vendor: %x, ID: %x\n", vendorID, get_device_id(bus, device, 0));
    check_function(bus, device, 0);
    
    uint8_t headerType = get_header_type(bus, device, 0);
    if ((headerType & 0x80) != 0)
    {
        // It's a multi-function device, so check remaining functions
        for (uint8_t function = 1; function < 8; function++)
        {
            if ((vendorID = get_vendor_id(bus, device, function)) != PCI_INVALID_DEVICE)
            {
                check_function(bus, device, function);
                LOG("Found m-function device. Vendor: %x, ID: %x\n", vendorID, get_device_id(bus, device, function));
            }
        }
    }
}

void check_function(uint8_t bus, uint8_t device, uint8_t function)
{
    uint8_t secondaryBus;
    uint8_t baseClass = get_base_class(bus, device, function);
    uint8_t subClass = get_subclass(bus, device, function);
    
    if ((baseClass == 0x6) && (subClass == 0x4))
    {
        secondaryBus = get_subclass(bus, device, function);
        check_bus(secondaryBus);
    }
}

void pci_init()
{    
    uint16_t headerType = get_header_type(0, 0, 0);
    if ((headerType & 0x80) == 0) // Single PCI host controller
        check_bus(0);
    else
    {
        // Multiple PCI host controllers
        for (uint8_t function = 0; function < 8; function++)
        {
            if (get_vendor_id(0, 0, function) != PCI_INVALID_DEVICE)
                break;
            
            check_bus(function);
        }
    }
}