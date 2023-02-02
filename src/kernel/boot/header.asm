MULTIBOOT2_HEADER_MAGIC equ 0xE85250D6

section .multiboot2.header  align=4
header_start:
    dd MULTIBOOT2_HEADER_MAGIC
    dd 0
    dd header_end - header_start
    dd -(MULTIBOOT2_HEADER_MAGIC + 0 + (header_end - header_start)) ; Checksum
    
    ; Tags
    dw 0
    dw 0
    dd 8

header_end: