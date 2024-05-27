#include <assert.h>
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

void __NO_RETURN__ __assertation_failed(const char *expr, const char *file, const char *function, uint32_t line)
{
    __CLI();
    
    // Prepare screen
    screen_clear(&BLUE);
    screen_set_colors(&BLUE, &WHITE);
    
    // Serial output
    printf("%s%s:%u: %s: Assertation `%s` failed.%s\n", COLOR_RED, file, line, function, expr, COLOR_DEFAULT);
    
    // Screen output
    fctprintf(sputc, NULL, "%s:%u: %s: Assertation `%s` failed.\n", file, line, function, expr);
    
    stacktrace_dump();
    __HCF();
}