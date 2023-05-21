#include <lib/math.h>

uint64_t pow(int base, int exp)
{
    uint64_t result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        
        exp >>= 1;
        if (!exp)
            break;
        
        base *= base;
    }

    return result;
}