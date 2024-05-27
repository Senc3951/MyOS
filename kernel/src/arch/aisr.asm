bits 32

KERNEL_DS equ 0x10

global _interrupt_handlers
extern _hl_interrupt_handler

section .text
int_common:
    cld     ; required by the sysV ABI
    
    ; store segment registers
    push ds
    push es
    push fs
    push gs
    
    ; store general registers
    pushad
    
    ; switch to kernel segment
    mov ax, KERNEL_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; call the kernel interrupt handler
    push esp
    call _hl_interrupt_handler
    add esp, 4
    
    ; restore general registers
    popad

    ; restore kernel segments
    pop gs
    pop fs
    pop es
    pop ds
    
    add esp, 8  ; num and error code
    iretd

%macro ISR 1
    isr_%1:
    %if !(%1 == 8 || (%1 >= 10 && %1 <= 14) || %1 == 17 || %1 == 21 || %1 == 29 || %1 == 30)
        push 0  ; push dummy error code
    %endif
        push %1 ; exception number
        jmp int_common
%endmacro

%assign i 0
%rep 256
    ISR i
%assign i i+1
%endrep

section .data
_interrupt_handlers:
    %assign i 0
    %rep 256
        dd isr_%+i
    %assign i i+1
    %endrep