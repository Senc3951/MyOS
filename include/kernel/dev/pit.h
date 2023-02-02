#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PIT_IRQ_PIN 0
#define PIT_DEFAULT_FREQUENCY 1000
#define PIT_COMMAND_PORT 0x43

typedef enum PIT_CHANNELS
{
    PIT_CHANNEL_0       = 0x40,
    PIT_CHANNEL_1       = 0x41,
    PIT_CHANNEL_2       = 0x42,
} PITChannels_t;

typedef enum PIT_Mode
{
    PIT_Binary = 0,
    PIT_BCD = 1
} PIT_Mode_t;

typedef enum PIT_OperatingMode
{
    PIT_InterruptOnTerminal     = 0,
    PIT_HardwareReTriggerable   = 0x2,
    PIT_RateGenerator           = 0x4,
    PIT_SquareWaveGenerator     = 0x6,
    PIT_SoftwareTriggeredStrobe = 0x8,
    PIT_HardwareTriggeredStrobe = 0xA
} PIT_OperatingMode_t;

typedef enum PIT_AccessMode
{
    PIT_LatchCount  = 0,
    PIT_lobyte      = 0xF,
    PIT_hibyte      = 0x20,
    PIT_lohibyte    = 0x30
} PIT_AccessMode_t;

typedef enum PIT_Channel
{
    PIT_Channel0 = 0,
    PIT_Channel1 = 0x40,
    PIT_Channel2 = 0x80,
    PIT_ReadBack = 0xC0
} PIT_Channel_t;

/**
 * @brief Initialize the PIT.
 * 
 * @param frequency Frequency to initialize the PIT with.
*/
void PIT_Initialize(const uint16_t frequency);

/**
 * @brief Suspend execution for a specified amount of milliseconds.
 * 
 * @param milliseconds Milliseconds to sleep.
*/
void PIT_Sleep(const size_t milliseconds);

/**
 * @brief Check if currently sleeping.
 * 
 * @return True if currently sleeping, False, otherwise.
*/
bool PIT_IsSleeping();

/**
 * @brief Get the time the os is running in seconds.
 * 
 * @return Uptime in seconds.
*/
size_t PIT_GetUptime();