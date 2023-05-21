#include <stdbool.h>
#include <lib/std.h>
#include <lib/math.h>
#include <lib/string.h>

static int int_to_str(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x % 10) + '0';
        x /= 10;
    }
 
    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';
    
    str[i] = '\0';
    strrev(str);
    
    return i;
}

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

void ftoa(double num, char *buffer, short a)
{
    int ipart = (int)num;
    double fpart = num - (double)ipart;
    if (fpart == 0.0)
    {
        itoa(ipart, buffer, 10, false);
        return;
    }
    
    int i = int_to_str(ipart, buffer, 0);
    if (a != 0)
    {
        buffer[i] = '.';
        fpart *= pow(10, a);
        
        int_to_str((int)fpart, buffer + i + 1, a);
    }
}