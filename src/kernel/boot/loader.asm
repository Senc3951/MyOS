; Access bits
PRESENT        equ 1 << 7
NOT_SYS        equ 1 << 4
EXEC           equ 1 << 3
DC             equ 1 << 2
RW             equ 1 << 1
ACCESSED       equ 1 << 0
 
; Flags bits
GRAN_4K       equ 1 << 7
SZ_32         equ 1 << 6
LONG_MODE     equ 1 << 5

global _start, gdt64, p4_table
extern long_mode_start

section .multiboot2.gdt64    align=4

gdt64:
    .null: equ $ - gdt64
        dq 0
    .code: equ $ - gdt64
        dd 0xFFFF                                   ; Limit & Base (low, bits 0-15)
        db 0                                        ; Base (mid, bits 16-23)
        db PRESENT | NOT_SYS | EXEC | RW            ; Access
        db GRAN_4K | LONG_MODE | 0xF                ; Flags & Limit (high, bits 16-19)
        db 0                                        ; Base (high, bits 24-31)
    .data: equ $ - gdt64
        dd 0xFFFF                                   ; Limit & Base (low, bits 0-15)
        db 0                                        ; Base (mid, bits 16-23)
        db PRESENT | NOT_SYS | RW                   ; Access
        db GRAN_4K | SZ_32 | 0xF                    ; Flags & Limit (high, bits 16-19)
        db 0                                        ; Base (high, bits 24-31)
    .tss: equ $ - gdt64
        dd 0x00000068
        dd 0x00CF8900
    .pointer:
        dw $ - gdt64 - 1
        dq gdt64

section .multiboot2.bss  nobits  write   align=4
align 0x1000
p4_table:
    resb 4096
p3_table_lower:
    resb 4096
p3_table_upper:
    resb 4096
mb_stack_bottom:
    resb 64
mb_stack_top:

section .multiboot2.text exec    align=16
bits 32
_start:
    cli
	cld
    
    mov esp, mb_stack_top
    mov edi, eax    ; Save multiboot Magic Number
    mov esi, ebx	; Save multiboot Information
    
    ; Checks
    call check_multiboot
    call check_cpuid
    call check_long_mode
    
    ; Setup Paging
    call setup_page_tables
    call enable_paging    

	; Load GDT
    lgdt [gdt64.pointer]
    jmp gdt64.code:long_mode_start
    
    hlt

error:
    mov dword [0xb8000], 0x4f524f45 ; 'ER'
    mov dword [0xb8004], 0x4f3a4f52 ; 'R:'
    mov dword [0xb8008], 0x4f204f20 ; '  '
    mov byte  [0xb800a], al         ; ASCII error byte
    hlt

check_multiboot:
    cmp eax, 0x36d76289
    jne .no_multiboot
    ret
.no_multiboot:
    mov al, "0"
    jmp error

check_cpuid:
    ; Copy FLAGS in to EAX via stack
    pushfd
    pop eax
 
    ; Copy to ECX as well for comparing later on
    mov ecx, eax
 
    ; Flip the ID bit
    xor eax, 1 << 21
 
    ; Copy EAX to FLAGS via the stack
    push eax
    popfd
 
    ; Copy FLAGS back to EAX (with the flipped bit if CPUID is supported)
    pushfd
    pop eax
 
    ; Restore FLAGS from the old version stored in ECX (i.e. flipping the ID bit
    ; back if it was ever flipped).
    push ecx
    popfd
 
    ; Compare EAX and ECX. If they are equal then that means the bit wasn't
    ; flipped, and CPUID isn't supported.
    xor eax, ecx
    jz .no_cpuid
    ret
.no_cpuid:
    mov al, "1"
    jmp error

check_long_mode:
    mov eax, 0x80000000    ; Set the A-register to 0x80000000.
    cpuid                  ; CPU identification.
    cmp eax, 0x80000001    ; Compare the A-register with 0x80000001.
    jb .no_long_mode         ; It is less, there is no long mode.

    ; use extended info to test if long mode is available
    mov eax, 0x80000001    ; Set the A-register to 0x80000001.
    cpuid                  ; CPU identification.
    test edx, 1 << 29      ; Test if the LM-bit, which is bit 29, is set in the D-register.
    jz .no_long_mode         ; They aren't, there is no long mode.
    ret
.no_long_mode:
    mov al, "2"
    jmp error

setup_page_tables:
    ; Setup P4 table
    mov eax, p3_table_lower    ; map first P4 entry to lower P3 table
    or eax, 0b11               ; present + writable
    mov [p4_table], eax

    mov eax, p3_table_upper    ; map last P4 entry to upper P4 table
    or eax, 0b11
    mov [p4_table + 511 * 8], eax

    ; Identity map first 2GB
    ; And map last 2GB to first 2GB
    ; Using 1GB huge pages in P3 table
    mov eax, 0x83       ; 0GB, present + writeable + huge
    mov [p3_table_lower], eax
    mov [p3_table_upper + 510 * 8], eax

    mov eax, 0x40000083 ; 1GB, present + writeable + huge
    mov [p3_table_lower + 8], eax
    mov [p3_table_upper + 511 * 8], eax

    ret

enable_paging:
    mov eax, p4_table       ; load physical address of P4 to cr3 register
    mov cr3, eax
    
    mov eax, cr4            ; enable PAE-flag in cr4 (Physical Address Extension)
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xC0000080     ; set the long mode bit in the EFER MSR (model specific register)
    rdmsr
    or eax, 1 << 8
    wrmsr
	
    mov eax, cr0            ; enable paging in the cr0 register
    or eax, 1 << 31
    mov cr0, eax

    ret