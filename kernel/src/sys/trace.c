#include <sys/trace.h>
#include <symbols.h>
#include <log.h>

uint8_t stacktrace(uintptr_t *addrptrs, const uint8_t max)
{
    uintptr_t *ebp;
    ebp = (uintptr_t *)__builtin_frame_address(0);
        
    if (max > MAX_TRACE)
        return 0;
    
    uint8_t i;
    for (i = 0; i < max && ebp; i++)
    {
        addrptrs[i] = ebp[1];       /* eip */
        ebp = (uintptr_t *)ebp[0];   /* ebp */
    }
    
    return i;
}

static inline char *traceAddress(uintptr_t *offset, const uintptr_t addr)
{
    // for (size_t i = 0; _kernel_symbols[i].addr != 0xffffffff; i++)
    // {
    //     if (_kernel_symbols[i].addr <= addr && _kernel_symbols[i + 1].addr > addr)
    //     {
    //         *offset = addr - _kernel_symbols[i].addr;
    //         return _kernel_symbols[i].name;
    //     }
    // }

    return NULL;
}

void stacktrace_dump()
{
    uintptr_t addrptrs[MAX_TRACE];
    uint8_t tc = stacktrace(addrptrs, MAX_TRACE);
    
    LOG_ERROR_NO_INFO("Stack Trace:\n");
    for (uint8_t i = 0; i < tc; i++)
    {
        uintptr_t offset;
        char *name = traceAddress(&offset, addrptrs[i]);
        
        if (!name)
            LOG_ERROR_NO_INFO("\tat %p <could not find function>\n", addrptrs[i]);
        else
            LOG_WARNING_NO_INFO("\tat %p: %s+%p\n", addrptrs[i], name, offset);
    }
}