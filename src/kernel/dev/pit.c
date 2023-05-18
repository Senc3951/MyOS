#include <io/io.h>
#include <dev/pit.h>
#include <arch/irq.h>

volatile size_t g_CountDown;
bool g_isSleeping = false;

static void interrupt_handler(struct interrupt_stack *stack)
{
    g_CountDown--;
}

void pit_init(const uint16_t frequency)
{
    uint16_t freq = 0x1234DE / frequency;
    if ((0x1234DE % frequency) > (frequency / 2))
        freq++;
    
    // Configure the PIT
    outb(PIT_COMMAND_PORT, PIT_Channel0 | PIT_lohibyte | PIT_SquareWaveGenerator | PIT_Binary);
    IO_WAIT();
    
    // Configure the frequency
    outb(PIT_CHANNEL_0, freq & 0xFF);  // Low byte
    IO_WAIT();
    outb(PIT_CHANNEL_0, (freq & 0xFF00) >> 8);     // High byte
    IO_WAIT();
    
    irq_register_interrupt(IRQ_PIT, interrupt_handler, true);
}

void pit_sleep(const size_t milliseconds)
{
    g_isSleeping = true;
    g_CountDown = milliseconds;

    while (g_CountDown > 0)
        HALT();
    
    g_isSleeping = false;
}

bool pit_is_sleeping()
{
    return g_isSleeping;
}