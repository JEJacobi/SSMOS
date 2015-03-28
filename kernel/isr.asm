; Interrupt handler system, all credit to the littleosbook for the macros.

;; NO ERROR HANDLING MACRO ;;
%macro no_error 1
global interrupt_handler_%1
interrupt_handler_%1:
	push dword 0
	push dword %1
	jmp common_handle
%endmacro

;; ERROR HANDLING MACRO ;;
%macro with_error 1
global interrupt_handler_%1
interrupt_handler_%1:
	push dword %1
	jmp common_handle
%endmacro

;; IRQ MACRO;;
%macro IRQ 2
global interrupt_handler_%1

; TODO

%endmacro

common_handle:
	pushad		; Save all registers.
	cld			; Clear DF because of the System V ABI.
	extern interrupt_handler
	call interrupt_handler ; And transfer to C-land handler.
	popad
	add esp, 8 	; Restore the esp to the pre-handling state.
	iret 		; And interrupt-return.

;	
; Interrupt definitions:
;

; Exceptions:
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