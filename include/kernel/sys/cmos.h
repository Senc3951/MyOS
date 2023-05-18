#pragma once

#include <stdint.h>

#define CMOS_ADDRESS_PORT   0x70
#define CMOS_DATA_PORT      0x71

typedef struct time
{
    uint16_t second;
    uint16_t minute;
    uint16_t hour;
    uint16_t day;
    uint16_t month;
    uint16_t year;
    uint16_t century;
} time_t;

/**
 * @brief Get the current time using CMOS.
 * 
 * @param t Struct to put the time into.
*/
void cmos_time(time_t *t);

/**
 * @brief Print a formatted message of the current time.
 * 
 * @param t Time struct to print.
*/
void cmos_print_formatted(const time_t *t);