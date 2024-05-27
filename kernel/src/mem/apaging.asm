bits 32

global x32_pd_load, x32_paging_enable

section .text
x32_pd_load:
    mov eax, [esp + 4]
    mov cr3, eax
    
    ret

x32_paging_enable:
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    
    ret