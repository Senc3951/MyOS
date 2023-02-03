#include <stddef.h>
#include <kernel/arch/irq.h>
#include <kernel/arch/pic.h>
#include <kernel/sys/panic.h>
#include <kernel/sys/logger.h>

ISRHandler g_IRQInterruptHandlers[IRQ15 - IRQ0];
bool g_SlaveEnabled = false;

void IRQ_RegisterInterrupt(const uint8_t interruptNumber, ISRHandler handler, const bool autoUnmask)
{
    LOGW("ISR", "Registering an Interrupt Handler for IRQ No. 0x%x\n", interruptNumber);
    if (interruptNumber >= 8 && !g_SlaveEnabled)
    {
        PIC_Unmask(IRQ_SLAVE);
        g_SlaveEnabled = true;
    }
    
    g_IRQInterruptHandlers[interruptNumber] = handler;
    if (autoUnmask)
        PIC_Unmask(interruptNumber);
}

void IRQ_InterruptHandler(InterruptStack_t *stack)
{
    uint8_t irqCode = stack->interruptNumber - IRQ0;
    if (g_IRQInterruptHandlers[irqCode] != NULL)
        g_IRQInterruptHandlers[irqCode](stack);
    else
        IKPANIC(stack, "Unhandled IRQ Detected.\nIRQ number: 0x%x, Interrupt number: 0x%x, Error Code: 0x%x.\n\n", irqCode, stack->interruptNumber, stack->errorCode);
    
    PIC_SendEOI(irqCode);
}