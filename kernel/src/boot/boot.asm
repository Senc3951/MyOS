extern __bss_start__, __bss_end__, __kernel_start__, __kernel_end__
extern kentry
global _entry

; kernel constants
STACK_SIZE                                  equ 0x4000

; constants
MULTIBOOT2_HEADER_MAGIC                     equ 0xe85250d6
MULTIBOOT2_HEADER_ARCHITECTURE              equ 0
MULTIBOOT2_HEADER_LENGTH                    equ (_entry - multiboot2_header)
MULTIBOOT2_HEADER_CHECKSUM                  equ -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT2_HEADER_ARCHITECTURE + MULTIBOOT2_HEADER_LENGTH)

; tag types
MULTIBOOT2_TAG_TERMINATE                    equ 0
MULTIBOOT2_TAG_INFORMATION_REQUEST          equ 1
MULTIBOOT2_TAG_ADDRESS                      equ 2
MULTIBOOT2_TAG_ENTRY_ADDRESS                equ 3
MULTIBOOT2_TAG_FLAGS                        equ 4
MULTIBOOT2_TAG_FRAMEBUFFER                  equ 5
MULTIBOOT2_TAG_MODULE_ALIGNMENT             equ 6

; request types
MULTIBOOT2_REQUEST_BOOT_COMMAND_LINE        equ 1
MULTIBOOT2_REQUEST_BOOT_LOADER_NAME         equ 2
MULTIBOOT2_REQUEST_MODULE                   equ 3
MULTIBOOT2_REQUEST_MEMORY_MAP               equ 6
MULTIBOOT2_REQUEST_FRAMEBUFFER_INFO         equ 8
MULTIBOOT2_REQUEST_SMBIOS_TABLES            equ 13
MULTIBOOT2_REQUEST_ACPI_OLD_RSDP            equ 14
MULTIBOOT2_REQUEST_ACPI_NEW_RSDP            equ 15

; tag flags
MULTIBOOT2_TAG_FLAG_OPTIONAL                equ 0x01
MULTIBOOT2_CONSOLE_FLAG_SUPPORT_TEXT_MODE   equ 0x02

section .text
multiboot2_header:
    ; header
    align 8
    dd MULTIBOOT2_HEADER_MAGIC
    dd MULTIBOOT2_HEADER_ARCHITECTURE
    dd MULTIBOOT2_HEADER_LENGTH
    dd MULTIBOOT2_HEADER_CHECKSUM

    ; address tag
    align 8
    dw MULTIBOOT2_TAG_ADDRESS
    dw MULTIBOOT2_TAG_FLAG_OPTIONAL
    dd 24
    dd multiboot2_header
    dd __kernel_start__
    dd __kernel_end__
    dd __bss_end__
    
    ; entry address tag
    align 8
    dw MULTIBOOT2_TAG_ENTRY_ADDRESS
    dw MULTIBOOT2_TAG_FLAG_OPTIONAL
    dd 12
    dd _entry

    ; flags tag
    align 8
    dw MULTIBOOT2_TAG_FLAGS
    dw MULTIBOOT2_TAG_FLAG_OPTIONAL
    dd 12
    dd MULTIBOOT2_CONSOLE_FLAG_SUPPORT_TEXT_MODE

    ; information request tag (required)
    align 8
    dw MULTIBOOT2_TAG_INFORMATION_REQUEST
    dw 0
    dd 24
    dd MULTIBOOT2_REQUEST_BOOT_COMMAND_LINE
    dd MULTIBOOT2_REQUEST_MODULE
    dd MULTIBOOT2_REQUEST_MEMORY_MAP
    dd MULTIBOOT2_REQUEST_FRAMEBUFFER_INFO

    ; information request tag (optional)
    align 8
    dw MULTIBOOT2_TAG_INFORMATION_REQUEST
    dw MULTIBOOT2_TAG_FLAG_OPTIONAL
    dd 24
    dd MULTIBOOT2_REQUEST_BOOT_LOADER_NAME
    dd MULTIBOOT2_REQUEST_SMBIOS_TABLES
    dd MULTIBOOT2_REQUEST_ACPI_OLD_RSDP
    dd MULTIBOOT2_REQUEST_ACPI_NEW_RSDP

    ; framebuffer tag (Delete to let GRUB boot in CGA text mode)
    align 8
    dw MULTIBOOT2_TAG_FRAMEBUFFER
    dw MULTIBOOT2_TAG_FLAG_OPTIONAL
    dd 20
    dd 0
    dd 0
    dd 32

    ; module alignment tag
    align 8
    dw MULTIBOOT2_TAG_MODULE_ALIGNMENT
    dw MULTIBOOT2_TAG_FLAG_OPTIONAL
    dd 8

    ; termination tag
    align 8
    dw MULTIBOOT2_TAG_TERMINATE
    dw 0
    dd 8

_entry:
    cli
    cld

    ; setup stack
    mov esp, stack_top
    xor ebp, ebp
    
    ; clear bss
    mov edi, __bss_start__
    mov ecx, __bss_end__
    sub ecx, edi    ; sub of bss
    shr ecx, 1      ; size / 2
    
    mov edx, eax
    xor eax, eax
    rep stosw

    ; store multiboot information
    push ebx    ; tags
    push edx    ; magic
    
    ; prepare environment
    call sse_enable
    call cpu_enable_features

    ; call c handler
    call kentry
    
    ; spin
    cli
.halt:
    hlt
    jmp .halt

sse_enable:
    mov eax, 0x1
    cpuid
    test edx, 1 << 25
    jz .no_sse
    
    mov eax, cr0
    and eax, 0xFFFB     ; clear coprocessor emulation CR0.EM
    or eax, 0x2         ; set coprocessor monitoring CR0.MP
    mov cr0, eax
    mov eax, cr4
    or eax, 3 << 9      ; set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
    mov cr4, eax
.no_sse:
    ret

cpu_enable_features:
    mov eax, cr0
    or eax, (1 << 16)   ; WP
    mov cr0, eax
    
    ret

section .bss
stack_buttom:
    resb STACK_SIZE
stack_top: