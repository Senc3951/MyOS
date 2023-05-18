#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PIT_DEFAULT_FREQUENCY 1000
#define PIT_COMMAND_PORT 0x43

enum pit_channels
{
    PIT_CHANNEL_0 = 0x40,
    PIT_CHANNEL_1 = 0x41,
    PIT_CHANNEL_2 = 0x42,
};

enum pit_mode
{
    PIT_Binary = 0,
    PIT_BCD = 1
};

enum pit_operating_mode
{
    PIT_InterruptOnTerminal     = 0,
    PIT_HardwareReTriggerable   = 0x2,
    PIT_RateGenerator           = 0x4,
    PIT_SquareWaveGenerator     = 0x6,
    PIT_SoftwareTriggeredStrobe = 0x8,
    PIT_HardwareTriggeredStrobe = 0xA
};

enum pit_access_mode
{
    PIT_LatchCount  = 0,
    PIT_lobyte      = 0xF,
    PIT_hibyte      = 0x20,
    PIT_lohibyte    = 0x30
};

enum pit_channel
{
    PIT_Channel0 = 0,
    PIT_Channel1 = 0x40,
    PIT_Channel2 = 0x80,
    PIT_ReadBack = 0xC0
};

/**
 * @brief Initialize the PIT.
 * 
 * @param frequency Frequency to initialize the PIT with.
*/
void pit_init(const uint16_t frequency);

/**
 * @brief Suspend execution for a specified amount of milliseconds.
 * 
 * @param milliseconds Milliseconds to sleep.
*/
void pit_sleep(const size_t milliseconds);