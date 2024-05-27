#include <dev/console.h>
#include <dev/fb/screen.h>
#include <log.h>

#define INPUT_BUF 128

typedef struct INPUT_HANDLER
{
  char buf[INPUT_BUF];
  uint32_t r;  // Read index
  uint32_t w;  // Write index
  uint32_t e;  // Edit index
} input_handler_t;

#define putc(c) screen_putc(c)
#define putc_all(c) ({              \
    putc(c);                        \
    LOG_NORMAL_NO_INFO("%c", c);    \
})

static input_handler_t g_input = { 0 };

void console_update(int (*getc)(void))
{
    #define C(x) ((x) - '@')
    
    int c;
    while ((c = getc()) >= 0)
    {
        switch (c)
        {
        case C('U'):  // Kill line
            while (g_input.e != g_input.w && g_input.buf[(g_input.e - 1) % INPUT_BUF] != '\n')
            {
                g_input.e--;
                putc_all(8);
            }
            
            break;
        case C('H'):
        case '\x7f':  // Backspace
            if (g_input.e != g_input.w)
            {
                g_input.e--;
                putc_all(8);
            }

            break;
        default:
            if (c != 0 && g_input.e - g_input.r < INPUT_BUF)
            {
                c = (c == '\r') ? '\n' : c;
                g_input.buf[g_input.e++ % INPUT_BUF] = c;
                putc_all(c);
                
                if (c == '\n' || c == C('D') || g_input.e == g_input.r + INPUT_BUF)
                {
                    g_input.w = g_input.e;
                }
            }
        }
    }
}