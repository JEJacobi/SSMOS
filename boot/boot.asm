;;;;;;;;;;;;;;;;;;;;;;;;
;; SSMOS - BOOTLOADER ;;
;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; CODE											;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;[org 0x7c00] ; Bootsector offset.
[bits 16] ; Start in 16-bit Real Mode.
extern kernel_main
global _start
jmp start ; Skip the BPB.

;; FAT BIOS PARAMETER BLOCK ;;;;;;;;;;;;;;;;;

db 0x0 ; Copied directly from the hex output of a formatted USB.
db 'SSMOS',0,0,0 ;
db 0x00,0x02,0x01,0x32,0x11,0x02,0x00,0x00,0x00,0x00,0xF8,0x00,0x00,0x3F,0x00,0xFF,0x00,0x80,0x00,0x00,0x00,0x00,0xD3,0x03,0x00,0x67,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x01,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x29,0x2F,0x2E,0x9D,0x36,0x4E,0x4F,0x20,0x4E,0x41,0x4D,0x45,0x20,0x20,0x20,0x20,0x46,0x41,0x54,0x33,0x32,0x20,0x20,0x20,0x33

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

start:
mov bp, LOWSTACK ; Quickly setup the stack.
mov sp, bp

mov [BOOTDRIVE], dl ; Record the bootdrive.

mov ah, 0x0 ; Set video mode to 80*25 VGA - 16 colors.
mov al, 0x3
int 0x10

mov bx, boot ; Bootloader running message.
call print_bios

mov ah, 0x1 ; Hide the hardware cursor
mov ch, 0010b
mov cl, 0x0
int 0x10

mov bx, 0xF ; Quickly test the stack.
push bx
mov bx, 0xE
push bx
mov bx, 0x0
pop bx
pop bx
cmp bx, 0xF
jne mem_error
jmp stack_good

mem_error: ; Something's really wrong.
mov bx, memerror
call print_bios
jmp $ ; Not even going to try to fix it, halt.

not_enough_memory: ; Not enough memory for the kernel to safely execute.
mov bx, notenoughmemory
call print_bios
jmp $ ; Throw out a message and halt.

stack_good: ; Everything's probably right.

xor ax, ax ; Get the lowmem, store it.
int 0x12 
jc mem_error ; Something's gone wrong.
mov [LOWMEM], ax
cmp ax, MINMEM
jle not_enough_memory ; If there's not enough memory for the kernel, don't bother trying.

mov ax, 0x2401
int 0x15 ; Try the BIOS activation of the A20 Line.
cmp ah, 0x86
jmp a20alt
jmp a20done

a20alt:

in al, 0x92
or al, 2 ; If that doesn't work, try a more traditional version.
out 0x92, al

a20done:

mov bx, memgood ; Memory seems to be working.
call print_bios

mov bx, loadkernel ; Kernel loading message.
call print_bios

; --- DISK LOAD ---
mov ah, 0x02			; Set the interrupt.
mov bx, SECONDSTAGE		; Load starting from the 2nd stage bootloader.
mov al, SECTORS 		; Load however many sectors specified (bootloader through to kernel).
mov dl, [BOOTDRIVE] 	; Set dl to load from the boot drive.
mov ch, 0x00
mov dh, 0x00			; Start reading from the second.
mov cl, 0x02 			; sector on disk (after the bootsector).

disk_read:
	int 0x13 ; Read interrupt.

jc read_error ; Carry flag means something's gone wrong, jump to handler.
jmp load_complete ; If successful, read complete.

read_error: ; Error reading the kernel.
	push bx ; Temporarily store bx.
	mov bx, readerror ; Print the error message.
	call print_bios ; TODO: Add specific error code messages.
	pop bx
	
	inc byte [READ_ATTEMPTS] ; Try to read again, up to MAX_ATTEMPTS.
	cmp byte [READ_ATTEMPTS], MAX_ATTEMPTS ; See if we've exceeded max attempts.
	jle disk_read ; If not, try again and hope it works.
	
	jmp $ ; If it still doesn't work, hang. Not much else to fix.

load_complete: ; If nothing's gone wrong.

mov bx, loadsuccess
call print_bios ; Throw out a load successful message.

call get_himem ; Get the amount of highmemory before switching to PM.

call setup_ps2 ; Setup the PS/2 controller.

; --- START PROTECTED MODE AND KERNEL ---
mov bx, switchpm
call print_bios ; PM switch message.

jmp switch_pm ; And switch to the PM function in the second stage bootloader.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; DATA											;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
BOOTDRIVE: dd 0				; What drive the bootloader was originally loaded from.
LOWMEM: dd 0				; How many KiB of low memory (~640k probably)
HIGHMEM: dd 0				; How many KiB of high memory before the 15mb hole.

READ_ATTEMPTS: db 0			; How many read attempts.
MINMEM: equ 0x1E0			; The minimum memory needed for the kernel to load and execute, (~480kb).
BOOTLOADER equ 0x7C00		; Location of the bootloader, usually 0x7C00.
SECONDSTAGE equ 0x7E00		; Location of the second stage bootloader, one sector after 1st stage (usually 0x7E00).
KERNEL_LOCATION equ 0x8000	; Location of the kernel, probably around 0x8000.
SECTORS equ 0x40			; How many sectors to read from the disk into kernelspace.
MAX_ATTEMPTS equ 0x5		; How many tries to read the disk.
LOWSTACK equ 0x7c00			; Location of the real mode stack.
HIGHSTACK equ 0x00007c00	; Location of the PM stack.
PS2CONTROLLER equ 0x64		; The PS/2 controller port.

boot: db 0x0a,0x0d,'Bootloader running...',0x0a,0x0d,0
loadkernel: db 0x0a,0x0d,'Loading kernel from disk...',0x0a,0x0d,0
readerror: db 0x0a,0x0d,'Disk read error!',0x0a,0x0d,0
memgood: db 0x0a,0x0d,'Memory checks passed.',0x0a,0x0d,0
notenoughmemory: db 0x0a,0x0d,'Insufficient memory!',0x0a,0x0d,0
memerror: db 0x0a,0x0d,'Memory error!',0x0a,0x0d,0
hex: db '0x0000',0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; FUNCTIONS									;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
[bits 16]
print_bios: ; bx is used as the start point of the string, and writes until NULL.
	mov ah, 0x0e ; set the interrupt
_char:
	mov al, [bx]
	int 0x10 ; print a char
	inc bx
	cmp byte [bx], 0x00
	jne _char
	ret

print_bios_hex:
	ret
	
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

times 510-($-$$) db 0
dw 0xaa55 ; Bootability.

;; SECOND STAGE ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

get_himem: ; Get the amount of high memory (before the 15m hole).
	xor eax, eax
	xor ebx, ebx ; Clear 'em all.
	xor ecx, ecx
	xor edx, edx
	mov ax, 0xE801
	int 0x15 ; Ask BIOS.
	jc himem_error ; Check for carry flag.
	cmp ah, 0x86 ; Make sure the function's supported.
	je himem_error
	cmp ah, 0x80 ; Check that the command is valid.
	je himem_error
	jcxz himem_other ; Check for cx = 0, if so, try ax.
	mov [HIGHMEM], cx ; If not, the value needed is in cx.
	jmp himem_successful

; HANDLE HIMEM:	
	
himem_other:
	mov [HIGHMEM], ax ; Using ax instead of cx.
	jmp himem_successful

himem_error:
	mov bx, himemerror
	call print_bios
	jmp $ ; Message and halt.
	
himem_successful:
	mov bx, himemsuccess
	call print_bios ; All's good.
	ret

; HANDLE PS/2:
	
setup_ps2:
	mov al, 0xA7
	out PS2CONTROLLER, al		; Disable the second PS/2 device, if it exists.
	
	;out PS2CONTROLLER, 0xAD		; Disable the first PS/2 device for initialization purposes.
	; TODO, modify the Controller Configuration Byte (see OSDev)
	;out PS2CONTROLLER, 0xAA		; Send self-test command to the PS/2 controller.
	jmp ps2_good
	
ps2_test:
	; TODO: Self test.
	
ps2_error:
	mov bx, ps2error
	call print_bios ; Something's gone wrong in the PS/2 self test.
	jmp $ ; Message and halt.

ps2_good:
	; RE ENABLE PS2 DEVICES
	mov bx, ps2good
	call print_bios
	ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; GDT - Global Descriptor Table				;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

gdt_begin: ; Simple-flat memory model.

dd 0x0 ; Null descriptor.
dd 0x0

gdt_data: ; Basically the same but data - 0x8
dw 0xffff
dw 0x0
db 0x0
db 10010010b
db 11001111b
db 0x0

gdt_code: ; Wizardry goes here - 0x10
dw 0xffff
dw 0x0
db 0x0
db 10011010b
db 11001111b
db 0x0

gdt_end: ; Apparently I need this.

gdt_descriptor: ; Precalculate the GDT descriptor.
dw gdt_end - gdt_begin - 1
dd gdt_begin

CODE_SEGMENT equ gdt_code - gdt_begin ; Precalculate the segment locations.
DATA_SEGMENT equ gdt_data - gdt_begin

;; SECOND STAGE DATA ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

loadsuccess: db 0x0a,0x0d,'Kernel successfully loaded!',0x0a,0x0d,0
switchpm: db 0x0a,0x0d,'Switching to Protected Mode...',0x0a,0x0d,0
himemsuccess: db 0x0a,0x0d,'Found high memory...',0x0a,0x0d,0
himemerror: db 0x0a,0x0d,'Error while getting high memory!',0x0a,0x0d,0
ps2good: db 0x0a,0x0d,'PS/2 controller self-test passed.',0x0a,0x0d,0
ps2error: db 0x0a,0x0d,'PS/2 self-test failed!',0x0a,0x0d,0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

switch_pm: ; Switch to Protected Mode.
	finit ; Initialize the FPU.
	cli ; Interrupts off.
	lgdt [gdt_descriptor] ; Pass the gdt.
	mov eax, cr0
	or eax, 0x1 ; Activate the PM bit.
	mov cr0, eax ; Set the control register for PM.
	
;; PROTECTED MODE STARTS HERE ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	jmp dword CODE_SEGMENT:setup_pm ; Farjump to clear pipeline, and set CS at the same time.
	
[bits 32]
setup_pm: ; Setup PM once in it.
	mov ax, DATA_SEGMENT
	mov ds, ax
	mov es, ax ; Assign all data segment registers.
	mov fs, ax ; to the valid memory thing.
	mov gs, ax
	mov ss, ax
	
	mov ebp, HIGHSTACK ; Setup the new protected mode stack.
	mov esp, ebp ; Old one's invalid, oh well.
	
	jmp start_kernel ; And back to main.
	
start_kernel:
	push dword [HIGHMEM] ; Push important things onto the stack... backwards because C.
	push dword [LOWMEM]
	push dword [BOOTDRIVE]
	call kernel_main ; And call kernel_main() in C land.
jmp $ ; Hang forever on return.