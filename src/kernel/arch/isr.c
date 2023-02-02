#include <kernel/symbols.h>
#include <kernel/arch/isr.h>
#include <kernel/arch/irq.h>
#include <kernel/arch/idt.h>
#include <kernel/sys/panic.h>
#include <kernel/sys/logger.h>

ISRHandler g_ISRInterruptHandlers[IDT_GATES];

static char* traceAddress(uintptr_t *offset, const uintptr_t addr);

static const char *g_EXCEPTIONS[] = {
    "Divide by zero error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "[Reserved]",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception ",
    "[Reserved]",
    "[Reserved]",
    "[Reserved]",
    "[Reserved]",
    "[Reserved]",
    "[Reserved]",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "[Reserved]"
};

void ISR_RegisterInterrupt(const uint8_t interruptNumber, ISRHandler handler)
{
    if (interruptNumber < IRQ0)
    {
        LOGW("ISR", "Registering a Handler for Interrupt No. 0x%x [%s]\n", interruptNumber, g_EXCEPTIONS[interruptNumber]);
    }
    else
        LOGW("ISR", "Registering a Handler for Interrupt No. 0x%x\n", interruptNumber);
    
    g_ISRInterruptHandlers[interruptNumber] = handler;
}

void ISR_InterruptHandler(InterruptStack_t *stack)
{
    uint64_t intNumber = stack->interruptNumber;
    if (g_ISRInterruptHandlers[intNumber] != NULL)
        g_ISRInterruptHandlers[intNumber](stack);
    else
    {
        if (!IS_IRQ(intNumber))
        {
            if (intNumber < IRQ0)
                ikpanic(stack, "%s Detected\nInterrupt number: 0x%x, Error Code: 0x%x.\n\n", g_EXCEPTIONS[intNumber], intNumber, stack->errorCode);
            else
                ikpanic(stack, "Interrupt number: 0x%x, Error Code: 0x%x.\n\n", g_EXCEPTIONS[intNumber], intNumber, stack->errorCode);
        }
        
        IRQ_InterruptHandler(stack);
    }
}

void ISR_DumpStackTrace(InterruptStack_t *stack, const uint16_t traceSize)
{
    uint16_t i = 0;
    StackFrame_t *current = (StackFrame_t *)stack->rbp;
    
    putc('\n');
    while (current != NULL && i++ < traceSize)
    {
        uintptr_t offset = 0;
        char* name = traceAddress(&offset, current->rip);

        if (name)
            kprintf("[0x%x] %s+0x%x\n", current->rip, name, offset);
        else
            kprintf("[0x%x]\n", current->rip);
        
        current = current->rbp;
    }

    if (i == 0)
        kprintf("Couldn't trace back.\n");
}

void ISR_DumpRegisters(InterruptStack_t *stack)
{
    kprintf("RAX = 0x%x , RBX = 0x%x , RCX = 0x%x , RDX = 0x%x\nRSI = 0x%x , RDI = 0x%x\n", stack->rax, stack->rbx, stack->rcx, stack->rdx, stack->rsi, stack->rdi);
    kprintf("CR0 = 0x%x , CR2=0x%x , CR3 = 0x%x\n", stack->cr0, stack->cr2, stack->cr3);
    kprintf("R8 = 0x%x , R9 = 0x%x , R10 = 0x%x , R11 = 0x%x , R12 = 0x%x\nR13 = 0x%x , R14 = 0x%x , R15=0x%x\n", stack->r8, stack->r9, stack->r10, stack->r11, stack->r12, stack->r13, stack->r14, stack->r15);
    kprintf("RIP = 0x%x , RSP = 0x%x , RBP = 0x%x , RFLAGS = 0x%x\n", stack->rip, stack->rsp, stack->rbp, stack->rflags);
}

char* traceAddress(uintptr_t *offset, const uintptr_t addr)
{
    for (int i = 0; _KernelSymbols[i].addr < UINT64_MAX; i++)
    {
        if (_KernelSymbols[i].addr < addr && _KernelSymbols[i + 1].addr >= addr)
        {
            *offset = addr - _KernelSymbols[i].addr;
            return _KernelSymbols[i].name;
        }
    }
    
    return NULL;
}