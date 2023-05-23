#include "string.h"

int memcmp(const void *d1, const void *d2, const size_t n)
{
    const uint8_t *c1 = (uint8_t *)d1;
    const uint8_t *c2 = (uint8_t *)d2;
    if (c1 == c2)
        return 0;
    
    for (size_t i = 0; i < n; i++, c1++, c2++)
    {
        if (*c1 != *c2)
            return (*c1 > *c2) ? 1 : -1;
    }
    
    return 0;
}

void memcpy(void *dest, const void *src, const size_t n)
{
    const uint8_t *s = (uint8_t *)src;
    uint8_t *d = (uint8_t *)dest;

    for (size_t i = 0; i < n; i++)
        d[i] = s[i];
}

void *memset(void *dest, const uint8_t c, size_t n)
{
    uint8_t *ps = dest;
    while (n--)
        *ps++ = c;
    
    return dest;
}

char *strncpy(char *dest, const char *src, size_t n)
{
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++)
        dest[i] = src[i];
    for (; i < n; i++)
        dest[i] = '\0';
    
   return dest;
}

size_t strlen(const char *s)
{
    const char *tmp;
    for (tmp = s; *tmp; ++tmp) ;
    
    return tmp - s;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    while (n && *s1 && (*s1 == *s2))
    {
        ++s1; ++s2;
        --n;
    }

    if (n == 0)
        return 0;
    
    return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

void strrev(char *s)
{
    int i = 0, j = strlen(s) - 1;
    char t;
    
    while (i < j)
    {
        t = s[i];
        s[i++] = s[j];
        s[j--] = t;
    }
}