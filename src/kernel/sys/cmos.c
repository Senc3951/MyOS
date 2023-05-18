#include <io/io.h>
#include <kprintf.h>
#include <sys/cmos.h>

#define CURRENT_YEAR 2023

#define GET_UPDATE_IN_PROGRESS_FLAG() ({ \
    outb(CMOS_ADDRESS_PORT, 0x0A); \
    (inb(CMOS_DATA_PORT) & 0x80); \
})

#define GET_RTC_REGISTER(reg) ({ \
    outb(CMOS_ADDRESS_PORT, reg); \
    inb(CMOS_DATA_PORT); \
})

uint8_t g_lSecond, g_lMinute, g_lHour, g_lDay, g_lMonth, g_lYear, g_lCentury, g_registerB;
int g_centuryRegister = 0x00;

void cmos_time(time_t *t)
{
    while (GET_UPDATE_IN_PROGRESS_FLAG()) ;                // Make sure an update isn't in progress
    t->second = GET_RTC_REGISTER(0x00);
    t->minute = GET_RTC_REGISTER(0x02);
    t->hour = GET_RTC_REGISTER(0x04);
    t->day = GET_RTC_REGISTER(0x07);
    t->month = GET_RTC_REGISTER(0x08);
    t->year = GET_RTC_REGISTER(0x09);
    if (g_centuryRegister != 0)
        t->century = GET_RTC_REGISTER(g_centuryRegister);

    do
    {
        g_lSecond = t->second;
        g_lMinute = t->minute;
        g_lHour = t->hour;
        g_lDay = t->day;
        g_lMonth = t->month;
        g_lYear = t->year;
        g_lCentury = t->century;

        while (GET_UPDATE_IN_PROGRESS_FLAG()) ;           // Make sure an update isn't in progress
        t->second = GET_RTC_REGISTER(0x00);
        t->minute = GET_RTC_REGISTER(0x02);
        t->hour = GET_RTC_REGISTER(0x04);
        t->day = GET_RTC_REGISTER(0x07);
        t->month = GET_RTC_REGISTER(0x08);
        t->year = GET_RTC_REGISTER(0x09);
        if (g_centuryRegister != 0)
            t->century = GET_RTC_REGISTER(g_centuryRegister);
    } while ((g_lSecond != t->second) || (g_lMinute != t->minute) || (g_lHour != t->hour) ||
               (g_lDay != t->day) || (g_lMonth != t->month) || (g_lYear != t->year) ||
               (g_lCentury != t->century));
    
    g_registerB = GET_RTC_REGISTER(0x0B);

    // Convert BCD to binary values if necessary
    if (!(g_registerB & 0x04))
    {
        t->second = (t->second & 0x0F) + ((t->second / 16) * 10);
        t->minute = (t->minute & 0x0F) + ((t->minute / 16) * 10);
        t->hour = ((t->hour & 0x0F) + (((t->hour & 0x70) / 16) * 10)) | (t->hour & 0x80);
        t->day = (t->day & 0x0F) + ((t->day / 16) * 10);
        t->month = (t->month & 0x0F) + ((t->month / 16) * 10);
        t->year = (t->year & 0x0F) + ((t->year / 16) * 10);
        if (g_centuryRegister != 0)
            t->century = (t->century & 0x0F) + ((t->century / 16) * 10);
    }
    
    // Convert 12 hour clock to 24 hour clock if necessary
    if (!(g_registerB & 0x02) && (t->hour & 0x80))
        t->hour = ((t->hour & 0x7F) + 12) % 24;
    
    // Calculate the full (4-digit) year
    if (g_centuryRegister != 0)
        t->year += t->century * 100;
    else
    {
        t->year += (CURRENT_YEAR / 100) * 100;
        if (t->year < CURRENT_YEAR)
            t->year += 100;
    }
}

void cmos_print_formatted(const time_t *t)
{
    kprintf("%u-%u-%u %u:%u:%u\n", t->year, t->month, t->day, t->hour, t->minute, t->second);
}