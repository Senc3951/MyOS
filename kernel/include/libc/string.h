#pragma once

#include <common.h>

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

inline size_t strlen(const char *s)
{
    const char *str;
    for (str = s; *str; str++) ;
    
    return str - s;
}

inline int strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

inline char *strcpy(char *dst, const char *src)
{
    if (!dst)
        return NULL;
    
    char *ptr = dst;
    while (*src)
        *dst++ = *src++;
    
    *dst = '\0';
    return ptr;
}