#include <dev/ps2/kbd.h>
#include <dev/ps2/translator.h>
#include <dev/console.h>
#include <arch/idt.h>
#include <arch/ioapic.h>
#include <io/io.h>
#include <panic.h>
#include <log.h>

#define KEYBOARD_DATA_PORT          0x60
#define KEYBOARD_STATUS_PORT        0x64
#define KEYBOARD_COMMAND_PORT       0x64

#define CONTROLLER_ENABLE_1         0xAE
#define CONTROLLER_ENABLE_2         0xA8
#define CONTROLLER_DISABLE_1        0xAD
#define CONTROLLER_DISABLE_2        0xA7
#define CONTROLLER_TEST             0xAA
#define CONTROLLER_TEST_RESULT      0x55
#define CONTROLLER_INTERFACE_TEST   0xAB

#define CONTROLLER_READ_CFG         0x20
#define CONTROLLER_WRITE_CFG        0x60
#define CFG_INT1                    0x1
#define CFG_INT2                    0x2
#define CFG_CLK1                    0x10
#define CFG_CLK2                    0x20
#define CFG_TRANS                   0x40

#define DEVICE_RESET                0xFF
#define DEVICE_SUCCESS              0xFA
#define KEYBOARD_SET_SCAN_CODE      0xF0
#define KEYBOARD_SCAN_CODE_1        1
#define KEYBOARD_ENABLE_SCANNING    0xF4
#define KEYBOARD_RESEND             0xFE
#define KEYBOARD_ACK                0xFA

#define WRITE_DATA(data) ({                         \
    while (inb(KEYBOARD_STATUS_PORT) & 0x2);        \
    outb(KEYBOARD_DATA_PORT, data);                 \
})
#define WRITE_COMMAND(data) ({                      \
    outb(KEYBOARD_COMMAND_PORT, data);              \
    __IO_WAIT();                                    \
})    
#define READ_DATA(data) ({                          \
    while ((inb(KEYBOARD_STATUS_PORT) & 0x1) == 0); \
    inb(KEYBOARD_DATA_PORT);                        \
})
#define WRITE_KEYBOARD(data) ({                                     \
    uint8_t res = 0, tries = 0;                                     \
    do {                                                            \
        WRITE_DATA(data);                                           \
        tries++;                                                    \
    } while (tries < 3 && (res = READ_DATA()) == KEYBOARD_RESEND);  \
    \
    res;                                                            \
})

static void interruptHandler(interrupt_frame_t *frame)
{
    UNUSED(frame);
    console_update(kbd_getc);
}

void kbd_init()
{
    // Disable device
    WRITE_COMMAND(CONTROLLER_DISABLE_1);
    WRITE_COMMAND(CONTROLLER_DISABLE_2);

    // Flush the buffer
    while (inb(KEYBOARD_COMMAND_PORT) & 1)
        inb(KEYBOARD_DATA_PORT);

    // Configure the device
    WRITE_COMMAND(CONTROLLER_READ_CFG);
    uint8_t config = inb(KEYBOARD_DATA_PORT);
    config |= (CFG_CLK1 | CFG_INT1);
    config &= ~(CFG_CLK2 | CFG_INT2 | CFG_TRANS);

    WRITE_COMMAND(CONTROLLER_WRITE_CFG);
    WRITE_DATA(config);

    // Test the controller
    WRITE_COMMAND(CONTROLLER_TEST);
    if (READ_DATA() != CONTROLLER_TEST_RESULT)
        panic("Controller tester failed");
    
    // Test the interface
    WRITE_COMMAND(CONTROLLER_INTERFACE_TEST);
    if (READ_DATA() != 0)
        panic("Controller interface tester failed");

    // Enable device
    WRITE_COMMAND(CONTROLLER_ENABLE_1);
    WRITE_COMMAND(CONTROLLER_ENABLE_2);

    // Reset device
    if (WRITE_KEYBOARD(DEVICE_RESET) != DEVICE_SUCCESS)
        panic("Unable to reset the Controller");
    if (WRITE_KEYBOARD(KEYBOARD_SET_SCAN_CODE) != KEYBOARD_ACK)
        panic("Unable to set the keyboard scan code");
    if (WRITE_KEYBOARD(KEYBOARD_SCAN_CODE_1) != KEYBOARD_ACK)
        panic("Unable to set the keyboard scan code");
    
    // Enable scanning
    if (WRITE_KEYBOARD(KEYBOARD_ENABLE_SCANNING) != KEYBOARD_ACK)
        panic("Unable to start the keyboard");
    
    // Register the interrupt
    register_interrupt(ISR_KBD, interruptHandler);
    ioapic_map_irq(ISR2IRQ(ISR_KBD), 0);
}

int kbd_getc()
{
    static uint32_t shift;
    static uint8_t *charcode[4] = { _Normalmap, _Shiftmap, _Ctlmap, _Ctlmap };
    uint32_t st, data, c;
    
    st = inb(KEYBOARD_STATUS_PORT);
    if ((st & 1) == 0)
        return -1;
    
    data = inb(KEYBOARD_DATA_PORT);
    if (data == 0xE0)
    {
        shift |= E0ESC;
        return 0;
    }
    else if (data & 0x80)
    {
        // Key released
        data = (shift & E0ESC ? data : data & 0x7F);
        shift &= ~(_Shiftcode[data] | E0ESC);
        return 0;
    }
    else if (shift & E0ESC)
    {
        // Last character was an E0 escape; or with 0x80
        data |= 0x80;
        shift &= ~E0ESC;
    }

    shift |= _Shiftcode[data];
    shift ^= _Togglecode[data];
    c = charcode[shift & (CTL | SHIFT)][data];
    
    if (shift & CAPSLOCK)
    {
        if ('a' <= c && c <= 'z')
            c += 'A' - 'a';
        else if ('A' <= c && c <= 'Z')
            c += 'a' - 'A';
    }
    
    return c;
}