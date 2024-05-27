bits 32

global x32_load_gdt, x32_load_idt

section .text
x32_load_gdt:
    ; load gdt
    mov eax, [esp + 4]
    lgdt [eax]
    
    ; reload segments
    ; reload code segment
    push dword [esp + 8]    ; cs
    lea eax, [rel .after_reload]
    push eax
    retfd
.after_reload:
    ; load data segment
    mov ax, [esp + 12]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ret

x32_load_idt:
    mov eax, [esp + 4]
    lidt [eax]
    
    ret