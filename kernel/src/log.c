#include <log.h>
#include <io/uart.h>
#include <libc/printf.h>

static const char *getColor(const uint32_t lvl)
{
    switch (lvl)
    {
        case LOG_LEVEL_WARNING:
            return COLOR_YELLOW;
        case LOG_LEVEL_ERROR:
            return COLOR_RED;
        default:
            return COLOR_DEFAULT;
    }
}

void __log_message(const bool pinfo, uint32_t lvl, const char *file, const uint32_t line, const char *fmt, ...)
{
#ifdef DEBUG
    va_list arg;
    va_start(arg, fmt);
    
    printf("%s", getColor(lvl));
    if (pinfo)
        printf("[%s:%u] ", file, line);
    
    vprintf(fmt, arg);
    printf("%s", COLOR_DEFAULT);
    
    va_end(arg);
#endif
}