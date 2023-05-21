#include <sys/fxsave.h>

char g_fxsaveRegion[512] __attribute__((aligned(16)));

void fxsave_init()
{
    asm volatile(" fxsave %0 "::"m"(g_fxsaveRegion));
}