#include <dev/pit.h>
#include <arch/irq.h>
#include <sys/panic.h>
#include <sys/logger.h>
#include <dev/keyboard/keyboard.h>
#include <dev/keyboard/translator.h>

#define MAX_TRIES 3

#define WRITE_DATA(data) ({ \
    while (inb(KEYBOARD_STATUS_PORT) & 0x2); \
    outb(KEYBOARD_DATA_PORT, data); \
})
#define WRITE_COMMAND(data) ({ \
    outb(KEYBOARD_COMMAND_PORT, data); \
    IO_WAIT(); \
})    
#define READ_DATA(data) ({\
    while ((inb(KEYBOARD_STATUS_PORT) & 0x1) == 0); \
    inb(KEYBOARD_DATA_PORT); \
})
#define WRITE_KEYBOARD(data) ({\
    uint8_t res = 0, tries = 0; \
    do {\
        WRITE_DATA(data); \
        tries++; \
    } while (tries < MAX_TRIES && (res = READ_DATA()) == KEYBOARD_RESEND); \
    \
    res; \
})

char g_LastChar = NO_KEY, g_CurrentChar = NO_KEY;
bool g_Caps = false, g_Released = false, g_Shift = false;

static void interrupt_handler(struct interrupt_stack *stack)
{
    char tmp;
    uint8_t data = inb(KEYBOARD_DATA_PORT);
    if (pit_is_sleeping())
        return;
    
    switch (data)
    {
        case 0:
        case INVALID_KEY:
            IKPANIC(stack, "Key detection error or internal buffer overrun.");
            break;
        case RELEASED_KEY:
            g_Released = true;
            break;
        case CAPS_KEY:
            if (!g_Released)
                g_Caps = !g_Caps;
            
            break;
        case RIGHT_SHIFT:
        case LEFT_SHIFT:
            g_Shift = !g_Released;
            break;
        case BACKSPACE:
            if (!g_Released)
                vga_delc();
            else
                g_Released = false;
            
            break;
        default:
            if (!g_Released)
            {
                tmp = translate(data, g_Caps, g_Shift);
                if (tmp != NO_KEY)
                    g_CurrentChar = tmp;
            }
            else
                g_Released = false;
            
            break;
    }
    
    if (g_LastChar == RELEASED_KEY)
        g_Released = false;
    
    g_LastChar = data;
}

static bool init_controller();
static bool init_keyboard_scan();

char keyboard_getc()
{
    while (g_CurrentChar == NO_KEY)
        HALT();
    
    char c = g_CurrentChar;
    g_CurrentChar = NO_KEY;
    
    return c;
}

bool keyboard_init()
{
    if (!init_controller() || !init_keyboard_scan())
        return false;
    
    irq_register_interrupt(IRQ_KEYBOARD, interrupt_handler, true);
    return true;
}

bool init_controller()
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
    {
        LOG("Controller tester failed.\n");
        return false;
    }

    // Test the interface
    WRITE_COMMAND(CONTROLLER_INTERFACE_TEST);
    if (READ_DATA() != 0)
    {
        LOG("Controller interface tester failed.\n");
        return false;
    }
    
    // Enable device
    WRITE_COMMAND(CONTROLLER_ENABLE_1);
    WRITE_COMMAND(CONTROLLER_ENABLE_2);
    
    // Reset device
    if (WRITE_KEYBOARD(DEVICE_RESET) != DEVICE_SUCCESS)
    {
        LOG("Unable to reset the Controller.\n");
        return false;
    }
    
    return true;
}

bool init_keyboard_scan()
{
    // Set the scan code.
    if (WRITE_KEYBOARD(KEYBOARD_SET_SCAN_CODE) != KEYBOARD_ACK)
    {
        LOG("Unable to set the keyboard's scan code.\n");
        return false;
    }
    if (WRITE_KEYBOARD(KEYBOARD_SCAN_CODE_2) != KEYBOARD_ACK)
    {
        LOG("Unable to set the keyboard's scan code.\n");
        return false;
    }
           
    // Enable scanning
    if (WRITE_KEYBOARD(KEYBOARD_ENABLE_SCANNING) != KEYBOARD_ACK)
    {
        LOG("Unable to start the keyboard.\n");
        return false;
    }
    
    return true;
}