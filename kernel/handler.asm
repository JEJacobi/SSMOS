; Interrupt handler system, all credit to the littleosbook for the macros.

;; NO ERROR HANDLING MACRO ;;
%macro no_error %1
global interrupt_handler_%1
interrupt_handler_%1:
	push dword 0
	push dword %1
	jmp common_handle
%endmacro

;; ERROR HANDLING MACRO ;;
%macro with_error %1
global interrupt_handler_%1
interrupt_handler_%1:
	push dword %1
	jmp common_handle
%endmacro

common_handle:
	pushad
	call interrupt_handler
	popad
	add esp, 8 	; Restore the esp to the pre-handling state.
	iret 		; And interrupt-return.
	
; Interrupt definitions:
	
no_error 0x80