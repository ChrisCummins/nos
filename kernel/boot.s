; boot.s - kernel entry location.

MBOOT_PAGE_ALIGN	equ 1<<0 ; Load kernel and modules on a page boundary
MBOOT_MEM_INFO		equ 1<<1 ; Provide kernel with memory info
MBOOT_HEADER_MAGIC	equ 0x1BADB002 ; Multiboot magic value
; NOTE: We do not use MBOOT_AOUT_KLUDGE. It means that GRUB does not
; pass us a symbol table. GRUB will also require our kernel be in elf32 format.
MBOOT_HEADER_FLAGS	equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM		equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

[BITS 32]		; All instructions should be 32 bit

[GLOBAL mboot]		; Make 'mboot' accessible from C
[EXTERN code]		; Start of the '.text' section
[EXTERN bss]		; Start of the .bss section
[EXTERN end]		; End of the last loadable section

mboot:
	dd	MBOOT_HEADER_MAGIC ; GRUB will search for this value on each
                                   ; 4-byte boundary in the kernel file
	dd	MBOOT_HEADER_FLAGS ; How GRUB should load your file/settings
	dd	MBOOT_CHECKSUM	   ; To ensure that the above values are correct
	dd	mboot		   ; Location of this descriptor
	dd	code		   ; stat of kernel .text section
	dd	bss		   ; end of kernel .data section
	dd	end		   ; end of kernel
	dd	start		   ; kernel entry point (initial EIP)

global start:function start.end-start ; Kernel entry point
[EXTERN kmain]			   ; Defined in ./main.c

start:
	cli
	mov     esp, stack         ; Grab our SBP
	push	ebx		   ; load multiboot header file
	mov     ebp, 0
	call	kmain
	jmp	$		; enter an infinite loop
.end:

section .bss
	resb 32768
stack: