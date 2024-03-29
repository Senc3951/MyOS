bits 32

global enable_sse
enable_sse:
    ; Check for SSE
    mov eax, 0x1
    cpuid
    test edx, 1<<25
    jz .no_sse

    ;now enable SSE and the like
    mov eax, cr0
    and ax, 0xFFFB		;clear coprocessor emulation CR0.EM
    or ax, 0x2			;set coprocessor monitoring  CR0.MP
    mov cr0, eax
    mov eax, cr4
    or ax, 3 << 9		;set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
    mov cr4, eax
.no_sse:
    ret