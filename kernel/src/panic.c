#include <panic.h>
#include <io/io.h>
#include <io/uart.h>
#include <dev/fb/screen.h>
#include <libc/printf.h>
#include <sys/trace.h>

static inline void sputc(char c, void *arg)
{
    UNUSED(arg);
    screen_putc(c);
}

void __NO_RETURN__ __panic(const char *file, const char *function, uint32_t line, const char *fmt, ...)
{
    __CLI();

    va_list arg;
    va_start(arg, fmt);
    
    // Prepare screen
    screen_clear(&BLUE);
    screen_set_colors(&BLUE, &WHITE);
    
    // Serial output
    printf("%s%s:%u: %s: panic. ", COLOR_RED, file, line, function);
    vprintf(fmt, arg);
    printf("%s\n", COLOR_DEFAULT);
    
    // Screen output
    fctprintf(sputc, NULL, "%s:%u: %s: panic. ", file, line, function);
    vfctprintf(sputc, NULL, fmt, arg);
    
    stacktrace_dump();
    va_end(arg);
    
    __HCF();
}