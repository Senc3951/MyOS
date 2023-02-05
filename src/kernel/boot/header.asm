MULTIBOOT2_HEADER_MAGIC equ 0xE85250D6
MULTIBOOT2_HEADER_TAG_END equ 0
MULTIBOOT2_HEADER_TAG_OPTIONAL equ 1
MULTIBOOT2_HEADER_TAG_FRAMEBUFFER equ 5

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

; This is to allow farme buffer
; align 8
;     dw MULTIBOOT2_HEADER_TAG_FRAMEBUFFER
;     dw MULTIBOOT2_HEADER_TAG_OPTIONAL
;     dd 20
;     dd 0 ; Let Grub decide the width and height of the frame buffer
;     dd 0
;     dd 32 ; 32 bit color depth
; align 8
;     dw MULTIBOOT2_HEADER_TAG_END
;     dw 0
;     dd 8

header_end: