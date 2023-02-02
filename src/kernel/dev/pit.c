#include <kernel/io/io.h>
#include <kernel/dev/pit.h>
#include <kernel/arch/irq.h>
#include <kernel/dev/display/vga.h>

static void interruptHandler(InterruptStack_t *stack);

volatile size_t g_CountDown;
size_t g_UpTime, g_TMP;     // UpTime will be in seconds
bool g_Sleeping;

void PIT_Initialize(const uint16_t frequency)
{
    uint16_t freq = 0x1234DE / frequency;
    if ((0x1234DE % frequency) > (frequency / 2))
        freq++;
    
    // Configure the PIT
    outb(PIT_COMMAND_PORT, PIT_Channel0 | PIT_lohibyte | PIT_SquareWaveGenerator | PIT_Binary);
    iowait();
    
    // Configure the frequency
    outb(PIT_CHANNEL_0, freq & 0xFF);  // Low byte
    iowait();
    outb(PIT_CHANNEL_0, (freq & 0xFF00) >> 8);     // High byte
    iowait();
    
    g_UpTime = g_TMP = 0;
    IRQ_RegisterInterrupt(PIT_IRQ_PIN, interruptHandler, true);
}

void PIT_Sleep(const size_t milliseconds)
{
    g_Sleeping = true;
    g_CountDown = milliseconds;

    while (g_CountDown > 0)
        __HALT();
    
    g_Sleeping = false;
}

bool PIT_IsSleeping()
{
    return g_Sleeping;
}

size_t PIT_GetUptime()
{
    return g_UpTime;
}

void interruptHandler(InterruptStack_t *stack)
{
    g_CountDown--;
    g_TMP++;
    
    // Every second, increment the counter
    if (g_TMP >= 1000)
    {
        g_UpTime++;
        g_TMP = 0;
    }
}