#include <dev/timer.h>
#include <arch/lapic.h>
#include <arch/ioapic.h>
#include <arch/idt.h>
#include <io/io.h>
#include <log.h>

#define LAPIC_TIMER_ONE_SHOT    (0 << 17)
#define LAPIC_TIMER_PERIODIC    (1 << 17)
#define LAPIC_TIMER_DIVIDER     3
#define CALIBRATION_MS          10

static size_t g_ticksPerMS;

static inline void _pitSleepNoInterrupt(const size_t ms)
{
    uint64_t total_count = 0x4A9 * ms;
	do
    {
        uint16_t count = MIN(total_count, 0xFFFFU);
        outb(0x43, 0x30);
        outb(0x40, count & 0xFF);
        outb(0x40, count >> 8);
        do
        {
            __PAUSE();
            outb(0x43, 0xE2);
        } while ((inb(0x40) & (1 << 7)) == 0);
        total_count -= count;
	} while ((total_count & ~0xFFFF) != 0);
}

static void interruptHandler(interrupt_frame_t *frame)
{
    UNUSED(frame);
    LOG("TIMER FINISHED\n");
}

void timer_init()
{
    register_interrupt(ISR_TIMER, interruptHandler);
    ioapic_map_irq(ISR2IRQ(ISR_TIMER), 0);

    // Configure the timer
    lapicw(LAPIC_TDCR, LAPIC_TIMER_DIVIDER);
    lapicw(LAPIC_TICR, UINT32_MAX);
    
    // Sleep & stop timer
    _pitSleepNoInterrupt(CALIBRATION_MS);
    lapicw(LAPIC_TIMER, LAPIC_MASKED);
    
    // Calculate how many ticks are in 1 ms
    uint32_t apicTicks = lapicr(LAPIC_TCCR);
    g_ticksPerMS = (UINT32_MAX - apicTicks) / CALIBRATION_MS;
    LOG_WARNING_INFO("Calculated %u ticks per ms in APIC timer\n", g_ticksPerMS);
}

void timer_oneshot(const uint64_t ms)
{
    lapicw(LAPIC_TDCR, LAPIC_TIMER_DIVIDER);
    lapicw(LAPIC_TIMER, LAPIC_TIMER_ONE_SHOT | ISR_TIMER);
    lapicw(LAPIC_TICR, g_ticksPerMS * ms);
}

void timer_periodic(const uint64_t ms)
{
    lapicw(LAPIC_TDCR, LAPIC_TIMER_DIVIDER);
    lapicw(LAPIC_TIMER, LAPIC_TIMER_PERIODIC | ISR_TIMER);
    lapicw(LAPIC_TICR, g_ticksPerMS * ms);
}