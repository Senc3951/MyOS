#include <stddef.h>
#include <arch/irq.h>
#include <arch/pic.h>
#include <sys/panic.h>
#include <sys/logger.h>

ISRHandler g_IRQInterruptHandlers[IRQ15 - IRQ0];
bool g_SlaveEnabled = false;

void irq_register_interrupt(const uint8_t interruptNumber, ISRHandler handler, const bool autoUnmask)
{
    LOG("Registering an Interrupt Handler for IRQ No. 0x%x\n", interruptNumber);
    if (interruptNumber >= 8 && !g_SlaveEnabled)
    {
        pic_unmask(IRQ_SLAVE);
        g_SlaveEnabled = true;
    }
    
    g_IRQInterruptHandlers[interruptNumber] = handler;
    if (autoUnmask)
        pic_unmask(interruptNumber);
}

void irq_interrupt_handler(struct interrupt_stack *stack)
{
    uint8_t irqCode = stack->interruptNumber - IRQ0;
    if (g_IRQInterruptHandlers[irqCode] != NULL)
        g_IRQInterruptHandlers[irqCode](stack);
    else
        IKPANIC(stack, "Unhandled IRQ Detected.\nIRQ number: 0x%x, Interrupt number: 0x%x, Error Code: 0x%x.\n\n", irqCode, stack->interruptNumber, stack->errorCode);
    
    pic_send_eoi(irqCode);
}