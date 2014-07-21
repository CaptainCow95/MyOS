global magic
global mb

; Declare constants used for creating a multiboot header.
MBALIGN     equ  1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ  1<<1                   ; provide memory map
FLAGS       equ  MBALIGN | MEMINFO      ; this is the Multiboot 'flag' field
MAGIC       equ  0x1BADB002             ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum of above, to prove we are multiboot

KERNEL_VIRTUAL_BASE equ 0xC0000000
KERNEL_PAGE_NUMBER equ ((KERNEL_VIRTUAL_BASE / 0x1000) / 1024)
 
; Declare a header as in the Multiboot Standard. We put this into a special
; section so we can force the header to be in the start of the final program.
; You don't need to understand all these details as it is just magic values that
; is documented in the multiboot standard. The bootloader will search for this
; magic sequence and recognize us as a multiboot kernel.
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

section .data
align 0x1000
BootPageDirectory:
	dd (BootPageTable - KERNEL_VIRTUAL_BASE)
	times (KERNEL_PAGE_NUMBER - 1) dd 0
	dd (BootPageTable - KERNEL_VIRTUAL_BASE)
	times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0

BootPageTable:
%assign i 0x00000003
%rep 1024
	dd i
%assign i i+0x1000
%endrep
 
; Currently the stack pointer register (esp) points at anything and using it may
; cause massive harm. Instead, we'll provide our own stack. We will allocate
; room for a small temporary stack by creating a symbol at the bottom of it,
; then allocating 16384 bytes for it, and finally creating a symbol at the top.
section .bootstrap_stack
align 4
stack_bottom:
times 0x4000 db 0
stack_top:
 
; The linker script specifies _start as the entry point to the kernel and the
; bootloader will jump to this position once the kernel has been loaded. It
; doesn't make sense to return from this function as the bootloader is gone.
section .text

global start
start equ (_start - KERNEL_VIRTUAL_BASE)

_start:
	mov	ecx, [BootPageDirectory - KERNEL_VIRTUAL_BASE]
	or ecx, 0x00000003
	mov [BootPageDirectory - KERNEL_VIRTUAL_BASE], ecx
	
	mov ecx, [BootPageDirectory - KERNEL_VIRTUAL_BASE + (KERNEL_PAGE_NUMBER * 4)]
	or ecx, 0x00000003
	mov [BootPageDirectory - KERNEL_VIRTUAL_BASE + (KERNEL_PAGE_NUMBER * 4)], ecx
	
	mov ecx, (BootPageDirectory - KERNEL_VIRTUAL_BASE)
	mov cr3, ecx
	
	mov ecx, cr0
	or ecx, 0x80000000
	mov cr0, ecx
	
	lea ecx, [HigherHalf]
	jmp ecx
	
HigherHalf:
	mov dword [BootPageDirectory], 0
	invlpg [0]
	
	; To set up a stack, we simply set the esp register to point to the top of
	; our stack (as it grows downwards).
	mov esp, stack_top
	
	mov [magic], eax
	mov [mb], ebx

	extern kernel_main
	call kernel_main
.hang:
	hlt
	jmp .hang

section .bss
align 4
magic: resd 1
mb: resd 1
