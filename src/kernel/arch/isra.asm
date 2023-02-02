bits 64

kernelDS equ 0x10

extern ISR_InterruptHandler

%macro ISR_EXCEPTION 2
    global INT%1
    INT%1:
        %if %2 == 1
            push qword 0    ; Push dummy error code
        %endif
        
        push qword %1
        jmp ISR_common
%endmacro

%macro pusha 0
    push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	mov r8, cr3
	push r8
	mov r8, cr2
	push r8
	mov r8, cr0
	push r8
	push rbp
	push rdi
	push rsi
	push rdx
	push rcx
	push rbx
	push rax
%endmacro

%macro popa 0
    pop rax
	pop rbx
	pop rcx
	pop rdx
	pop rsi
	pop rdi
	pop rbp
	add rsp, 24
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
%endmacro

%include "arch/isra.inc"

ISR_common:
	cld
    pusha
		
	xor rax, rax
	mov ax, ds
	push rax
	
	; Handle the interrupt inside Kernel's DS
	mov ax, kernelDS
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	; Pass a Pointer to all the information
    mov rdi, rsp
    call ISR_InterruptHandler
    
	; Restore Data segments
	pop rax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
		
    popa
    add rsp, 16     ; Remove Interrupt number and Error code    
	iretq

global interruptHandlers
interruptHandlers:
	%macro INT_NAME 1
		dq INT%1
	%endmacro
		
	%assign i 0
	%rep 47
	INT_NAME i
	%assign i i+1
	%endrep