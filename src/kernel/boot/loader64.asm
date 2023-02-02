global long_mode_start, stack_bottom

section .multiboot2.text exec   align=16

bits 64
long_mode_start:
    mov rsp, stack_top
    
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    extern kmain
    call kmain
.halt:
    cli
    hlt
    jmp .halt

section .bss

align 0x1000
stack_bottom:
    resb 4096 * 2
stack_top: