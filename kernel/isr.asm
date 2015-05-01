; Interrupt handler system, all credit to the littleosbook for the macros.

;; NO ERROR HANDLING MACRO ;;
%macro no_error 1
global interrupt_handler_%1
interrupt_handler_%1:
	push dword [BUFFER]
	push dword 0
	push dword %1
	jmp common_handle
%endmacro

;; ERROR HANDLING MACRO ;;
%macro with_error 1
global interrupt_handler_%1
interrupt_handler_%1:
	push dword [BUFFER]
	push dword %1
	jmp common_handle
%endmacro

;; IRQ MACRO;;
%macro IRQ 2
global interrupt_handler_%1

; TODO

%endmacro

BUFFER: dd 0x0

common_handle:
	cli
	pushfd		; Save EFLAGS
	pushad		; Save all registers.
	cld			; Clear DF because of the System V ABI.
	extern interrupt_handler
	call interrupt_handler ; And transfer to C-land handler.
	mov [BUFFER], eax ; Buffer the return value around popad.
	;xchg bx, bx
	popad		; Restore registers and EFLAGS.
	popfd
	mov eax, [BUFFER] ; Unbuffer, and clear edx since it's the other return register.
	xor edx, edx
	add esp, 8 	; Restore the esp to the pre-handling state.
	pop dword [BUFFER]
	sti
	iret 		; And interrupt-return.

;	
; Interrupt definitions:
;

; Exceptions (0x0 - 0x1F):
no_error	0x0
no_error	0x1
no_error	0x2
no_error	0x3
no_error	0x4
no_error	0x5
no_error	0x6
no_error	0x7
with_error	0x8
no_error	0x9
with_error	0xA
with_error	0xB
with_error	0xC
with_error	0xD
with_error	0xE
no_error	0xF
no_error	0x10
with_error	0x11
no_error	0x12
no_error	0x13
no_error	0x14
; -- RESERVED (0x15 - 0x1D) --
with_error	0x1E
; -- RESERVED (0x1F) --


; IRQs (0x20 - 0x2F):
no_error	0x20
no_error	0x21
no_error	0x22
no_error	0x23
no_error	0x24
no_error	0x25
no_error	0x26
no_error	0x27
no_error	0x28
no_error	0x29
no_error	0x2A
no_error	0x2B
no_error	0x2C
no_error	0x2D
no_error	0x2E
no_error	0x2F


; System Calls (0x80):
no_error	0x80

; Function for loading the IDT given the idtptr in interrupts.c
global load_idt
extern idtptr
load_idt:
	lidt [idtptr]
	ret