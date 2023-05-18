#include <stddef.h>
#include <arch/isr.h>
#include <arch/idt.h>
#include <sys/logger.h>

ISRHandler g_ISRInterruptHandlers[IDT_GATES];

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

void isr_register_interrupt(const uint8_t interruptNumber, ISRHandler handler)
{
    if (interruptNumber < 0x20)
        LOG("Registering a Handler for Interrupt No. 0x%x [%s]\n", interruptNumber, g_EXCEPTIONS[interruptNumber]);
    else
        LOG("Registering a Handler for Interrupt No. 0x%x\n", interruptNumber);
    
    g_ISRInterruptHandlers[interruptNumber] = handler;
}

void isr_interrupt_handler(struct interrupt_stack *stack)
{
    uint64_t intNumber = stack->interruptNumber;
    if (g_ISRInterruptHandlers[intNumber] != NULL)
        g_ISRInterruptHandlers[intNumber](stack);
    else
        isr_dump_registers(stack);
}

void isr_dump_registers(struct interrupt_stack *stack)
{
    kprintf("RAX = 0x%x , RBX = 0x%x , RCX = 0x%x , RDX = 0x%x\nRSI = 0x%x , RDI = 0x%x\n", stack->rax, stack->rbx, stack->rcx, stack->rdx, stack->rsi, stack->rdi);
    kprintf("CR0 = 0x%x , CR2=0x%x , CR3 = 0x%x\n", stack->cr0, stack->cr2, stack->cr3);
    kprintf("R8 = 0x%x , R9 = 0x%x , R10 = 0x%x , R11 = 0x%x , R12 = 0x%x\nR13 = 0x%x , R14 = 0x%x , R15=0x%x\n", stack->r8, stack->r9, stack->r10, stack->r11, stack->r12, stack->r13, stack->r14, stack->r15);
    kprintf("RIP = 0x%x , RSP = 0x%x , RBP = 0x%x , RFLAGS = 0x%x\n", stack->rip, stack->rsp, stack->rbp, stack->rflags);
}