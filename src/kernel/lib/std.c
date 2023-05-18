#include <stdbool.h>
#include <lib/std.h>
#include <lib/string.h>

void itoa(int64_t num, char *buffer, const int base, const bool caps)
{
    int i = 0, rem;
    bool negative = false;
    char hexstart = (caps) ? 'A' : 'a';

    if (buffer == NULL)
        return;

    // Handle 0
    if (num == 0)
    {
        buffer[i] = '0';
        buffer[++i] = '\0';
        
        return;
    }

    // Only treat base 10 numbers as signed
    if (base == 10 && num < 0)
    {
        negative = true;
        num *= -1;
    }

    // Convert number to string
    while (num > 0)
    {
        rem = num % base;
        buffer[i++] = (rem < 10) ? rem + '0' : (rem - 10) + hexstart;
        num /= base;
    }

    // Append '-'
    if (negative)
        buffer[i++] = '-';

    // Null terminate
    buffer[i] = '\0';

    // Reverse string
    strrev(buffer);
}

void uitoa(uint64_t num, char *buffer, const int base, const bool caps)
{
    int i = 0, rem;
    char hexstart = (caps) ? 'A' : 'a';

    if (buffer == NULL)
        return;

    // Handle 0
    if (num == 0)
    {
        buffer[i] = '0';
        buffer[++i] = '\0';
        
        return;
    }

    // Convert number to string
    while (num > 0)
    {
        rem = num % base;
        buffer[i++] = (rem < 10) ? rem + '0' : (rem - 10) + hexstart;
        num /= base;
    }
    
    // Null terminate
    buffer[i] = '\0';

    // Reverse string
    strrev(buffer);
}