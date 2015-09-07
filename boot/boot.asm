;;;;;;;;;;;;;;;;;;;;;;;;
;; SSMOS - BOOTLOADER ;;
;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; CODE                                         ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;[org 0x7c00] ; Bootsector offset.
; Commented out because compiling as an ELF file means externally defined org.

[bits 16] ; Start in 16-bit Real Mode.
extern kernel_main  ; This is the main kernel function we'll jump to later.
global _start       ; Useless right now, kept around in case it's ever needed.
jmp 0x0000:start    ; Skip the BPB, and ensure CS = 0, just in case the BIOS loaded it weirdly.

;; FAT BIOS PARAMETER BLOCK ;;;;;;;;;;;;;;;;;

db 0x0 ; TODO: Generate all this through mkdosfs.
db 'SSMOS',0,0,0 ;
db 0x00,0x02,0x01,0x32,0x11,0x02,0x00,0x00,0x00,0x00,0xF8,0x00,0x00,0x3F,0x00,0xFF,0x00,0x80,0x00,0x00,0x00,0x00,0xD3,0x03,0x00,0x67,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x01,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x29,0x2F,0x2E,0x9D,0x36,0x4E,0x4F,0x20,0x4E,0x41,0x4D,0x45,0x20,0x20,0x20,0x20,0x46,0x41,0x54,0x33,0x32,0x20,0x20,0x20,0x33

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; --- INITIALIZATION ---

start:              ; The jump target immediately after the FAT BPB.

mov ax, 0
mov ss, ax
mov bp, LOWSTACK    ; Quickly setup the stack and segments.
mov sp, bp
xor ax, ax

mov [BOOTDRIVE], dl ; Record the bootdrive.

mov ah, 0x0         ; Set video mode to 80*25 VGA - 16 colors.
mov al, 0x3         ; This might change if a proper graphical driver ever gets in.
int 0x10

mov bx, boot        ; Bootloader running message.
call print_bios

mov ah, 0x1         ; Hide the hardware cursor.
mov ch, 0010b
mov cl, 0x0
int 0x10            ; Hacky as all hell, does get rid of the BIOS cursor reasonably quickly, though.

xor ax, ax              ; Get the lowmem, store it.
int 0x12 
jc mem_error            ; Something's gone wrong.
mov [LOWMEM], ax
cmp ax, MINMEM
jle not_enough_memory   ; If there's not enough memory for the kernel, don't bother trying.
                        ; Note that this only tests for LOWMEM, the kernel is responsible for handling >1MB stuff.
jmp mem_good            ; Otherwise, continue.

not_enough_memory:      ; Not enough memory for the kernel to safely execute.
    mov bx, notenoughmemory
    call print_bios
    jmp $               ; Throw out a message and halt.

mem_error:              ; Something's gone -really- wrong.
    mov bx, memerror
    call print_bios
    jmp $
    
mem_good:               ; Everything's good with the memory.
    
mov ax, 0x2401
int 0x15                ; Try the BIOS activation of the A20 Line.
cmp ah, 0x86
jmp a20alt
jmp a20done

a20alt:                 ; If that doesn't work, try a more traditional version.

in al, 0x92
or al, 2            
out 0x92, al

a20done:                ; At this point, if the A20 still isn't on, there's not much more else to try.

; --- LOAD SECOND STAGE AND KERNEL FROM DISK ---

; First, load the rest of the bootloader without any fancy conversions.
; We just need more space to work in.

mov ah, 0x2             ; Set the interrupt.
mov bx, SECONDSTAGE     ; Load starting from the 2nd stage bootloader.
mov al, LOADERSECTORS   ; Load however many sectors specified for the second stage loader.
mov dl, [BOOTDRIVE]     ; Set dl to load from the boot drive.
mov ch, 0x0
mov dh, 0x0             ; Start reading from the second.
mov cl, 0x2             ; sector on disk (after the bootsector).

call read_disk          ; Read the loader or fail trying.
call get_diskinfo       ; Next, get disk info (sectors per track and number of heads) BEFORE calling lba->chs

; Now that the second stage loader is in memory at 0x7E00-0x8000, load the kernel.

kernel_load:                            ; We'll loop back here for each sector load.
    mov ax, word [CURRENT_OFFSET]       ; Load the segment register used by 13h from memory.
    mov es, ax
    mov ax, word [CURRENT_SECTOR]       ; Load the LBA sector to read into ax.
    call lba_chs                        ; Convert it to CHS format, which is in memory.
    xor ax, ax
    mov ah, 0x2
    mov bx, 0x0                         ; We'll be loading into segments, not offsets.
    mov al, 0x1                         ; Only load one sector at a time.
    mov dl, [BOOTDRIVE]                 ; Make sure the boot drive is selected.
    mov ch, byte [cyl]                  ; Load cylinder from memory.
    and ch, 0xFF                        ; AND cylinders by 0xFF.
    mov dh, byte [head]                 ; Load heads from memory.
    mov cl, byte [sector]               ; Load sectors from memory.
                                        ; NOTE: This is technically illegal for high cylinder values.
                                        
    call read_disk                      ; And read the disk.
    
    add word [CURRENT_OFFSET], SECTORSIZE
    add word [CURRENT_SECTOR], 1        ; Increment the current offset to read to and the last sector that's been read.
    cmp word [CURRENT_SECTOR], TOTALSECTORS
    jle kernel_load                     ; Jump back to kernel_load if there's more sectors to read.

mov bx, loadsuccess ; Otherwise, we're done.
call print_bios     ; Throw out a load successful message.

call get_himem      ; Get the amount of highmemory before switching to PM.

call setup_ps2      ; Setup the PS/2 controller.

; --- START PROTECTED MODE AND KERNEL ---
mov bx, switchpm
call print_bios ; PM switch message.

jmp switch_pm ; And switch to the PM function in the second stage bootloader.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; DATA                                         ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; MEMORY AND INIT:

BOOTDRIVE: dd 0             ; What drive the bootloader was originally loaded from.
LOWMEM: dd 0                ; How many KiB of low memory (~640k probably)
HIGHMEM: dd 0               ; How many KiB of high memory before the 15mb hole.

MINMEM: equ 0x1E0           ; The minimum memory needed for the kernel to load and execute, (~480kb).

LOWSTACK equ 0x7c00         ; Location of the real mode stack.
HIGHSTACK equ 0x00007c00    ; Location of the PM kernel stack.


; LOADING:

CURRENT_SECTOR: dw 0x2      ; What LBA sector we're reading right now.
CURRENT_OFFSET: dw 0x0800   ; What our current offset target is, currently starting at 0x8000.
READ_ATTEMPTS: db 0x0       ; How many disk read attempts so far.

MAX_ATTEMPTS equ 0x5        ; How many tries to read the disk before giving up.

SECONDSTAGE equ 0x7E00      ; Location of the second stage bootloader, one sector after 1st stage (usually 0x7E00-0x8000).
LOADERSECTORS equ 0x1       ; Size of the second stage bootloader in sectors.

TOTALSECTORS equ 0x50       ; What LBA sector to stop at.
SECTORSIZE equ 0x020        ; Standard disk sectors are 512-bytes long, and since this is an offset, / 16.

; MISC:
    
PS2CONTROLLER equ 0x64      ; The PS/2 controller port.

; STRINGS:

boot: db 0x0a,0x0d,'Bootloader running...',0x0a,0x0d,0
loadkernel: db 0x0a,0x0d,'Loading kernel from disk...',0x0a,0x0d,0
readerror: db 0x0a,0x0d,'Disk read error!',0x0a,0x0d,0
memgood: db 0x0a,0x0d,'Memory checks passed.',0x0a,0x0d,0
memerror:db 0x0a,0x0d,'Memory read error!',0x0a,0x0d,0
notenoughmemory: db 0x0a,0x0d,'Insufficient memory!',0x0a,0x0d,0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; FUNCTIONS                                    ;;;
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

read_disk: ; Interrupt values should be pre-set before calling this, it just handles any errors.
    mov byte [READ_ATTEMPTS], 0x0   ; Clear any previous reads' attempts.
    
    disk_read:
        int 0x13                    ; Read interrupt.

    jc read_error                   ; Carry flag means something's gone wrong, jump to handler.
    ret                             ; If successful, read is complete and return.

    read_error:                     ; Error reading the kernel.
        push bx                     ; Temporarily store bx.
        mov bx, readerror           ; Print the error message.
        call print_bios
        pop bx
    
        inc byte [READ_ATTEMPTS]    ; Try to read again, up to MAX_ATTEMPTS.

        cmp byte [READ_ATTEMPTS], MAX_ATTEMPTS 

                                    ; See if we've exceeded max attempts.
        jle disk_read               ; If not, try again and hope it works.
    
        jmp $                       ; If it still doesn't work, hang. Not much else to fix.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

times 510-($-$$) db 0
dw 0xaa55 ; Bootability. This also marks the very end of the first stage loader.

;; SECOND STAGE ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

get_himem:              ; Get the amount of high memory (before the 15m hole).
    xor eax, eax
    xor ebx, ebx        ; Clear 'em all.
    xor ecx, ecx
    xor edx, edx
    mov ax, 0xE801
    int 0x15            ; Ask BIOS.
    jc himem_error      ; Check for carry flag.
    cmp ah, 0x86        ; Make sure the function's supported.
    je himem_error
    cmp ah, 0x80        ; Check that the command is valid.
    je himem_error
    jcxz himem_other    ; Check for cx = 0, if so, try ax.
    mov [HIGHMEM], cx   ; If not, the value needed is in cx.
    jmp himem_successful

; HANDLE HIMEM: 
    
himem_other:
    mov [HIGHMEM], ax   ; Using ax instead of cx.
    jmp himem_successful

himem_error:
    mov bx, himemerror
    call print_bios
    jmp $ ; Message and halt.
    
himem_successful:
    mov bx, himemsuccess
    call print_bios     ; All's good.
    ret

; HANDLE PS/2:
    
setup_ps2:                      ; TODO: This whole area really needs a complete overhaul.
    mov al, 0xA7
    out PS2CONTROLLER, al       ; Disable the second PS/2 device, if it exists.
    
    ;out PS2CONTROLLER, 0xAD    ; Disable the first PS/2 device for initialization purposes.
                                ; TODO, modify the Controller Configuration Byte (see OSDev)
    ;out PS2CONTROLLER, 0xAA    ; Send self-test command to the PS/2 controller.
    jmp ps2_good
    
ps2_test:
                                ; TODO: Self test.
    
ps2_error:
    mov bx, ps2error
    call print_bios             ; Something's gone wrong in the PS/2 self test.
    jmp $                       ; Message and halt.

ps2_good:                       ; Re-enable PS2 devices if everything's good.
    mov bx, ps2good
    call print_bios
    ret
    
; GET DISK INFO for LBA->CHS CONVERSION:
get_diskinfo:
    pusha
    
    xor ax, ax
    mov ah, 0x8             ; Call the BIOS for the sectors per track and number of heads.
    mov dl, [BOOTDRIVE]
    int 0x13                ; On return, number of heads is dh - 1.
    inc dh                  ; Increment by one to bring it to 1-indexed CHS format.
    mov byte [numhd], dh    ; And move it into memory.
    and cl, 0x3F            ; Bitwise AND CL to get the sectors per track.
    mov byte [spt], cl      ; And move that into memory.    
    
    popa
    ret
    
; HANDLE LBA->CHS CONVERSION:

lba_chs:            ; Takes AX as LBA address, converts it, and saves the value in memory.
    push ax         ; At [cyl] : [head] : [sector], but [spt] and [numhd] MUST be already setup.    
    push dx

    xor dx, dx                  ; Make sure there isn't anything in dx interfering with div.
    div word [spt]              ; Divide LBA (ax) / Sectors Per Track [spt]
    mov bx, ax                  ; Move TEMP from ax to bx. This will be used later.
    mov byte [sector], dl       ; Store the remainder (modulus) in sectors.
    inc byte [sector]           ; And increment it once, since CHS indexes at 1.
    xor dx, dx                  ; Clear dx again for the next division.

    div word [numhd]            ; Next, divide ax (TEMP) / [numhd] (number of heads)
    mov byte [head], dl         ; Store the remainder in [head].
    mov byte [cyl], al          ; And the quotient in [cyl].

    pop dx
    pop ax
    ret                         ; And that's the sector, head, and cylinder.  


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; GDT - Global Descriptor Table                ;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

gdt_begin:      ; Simple-flat memory model.

dd 0x0          ; Null descriptor.
dd 0x0

gdt_data:       ; The main data segment - 0x8
dw 0xffff
dw 0x0
db 0x0
db 10010010b
db 11001111b
db 0x0

gdt_code:       ; Wizardry goes here (and code!) - 0x10
dw 0xffff
dw 0x0
db 0x0
db 10011010b
db 11001111b
db 0x0

gdt_end:

gdt_descriptor:                         ; Precalculate the GDT descriptor.
dw gdt_end - gdt_begin - 1
dd gdt_begin

CODE_SEGMENT equ gdt_code - gdt_begin   ; Precalculate the segment locations.
DATA_SEGMENT equ gdt_data - gdt_begin

;; SECOND STAGE DATA ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; LBA REQUIRED INFORMATION:
; (fill this out before calling lba->chs)

spt:    dw 0x0  ; Sectors per track of the boot disk.
numhd:  dw 0x0  ; Number of heads of the boot disk.


; LBA CONVERSION OUTPUTS:

sector: db 0x0  ; Current sector to read.
head:   db 0x0  ; Current head to read.
cyl:    db 0x0  ; Current cylinder to read.


loadsuccess: db 0x0a,0x0d,'Kernel successfully loaded!',0x0a,0x0d,0
switchpm: db 0x0a,0x0d,'Switching to Protected Mode...',0x0a,0x0d,0
himemsuccess: db 0x0a,0x0d,'Found high memory...',0x0a,0x0d,0
himemerror: db 0x0a,0x0d,'Error while getting high memory!',0x0a,0x0d,0
ps2good: db 0x0a,0x0d,'PS/2 controller self-test passed.',0x0a,0x0d,0
ps2error: db 0x0a,0x0d,'PS/2 self-test failed!',0x0a,0x0d,0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

switch_pm:                  ; Switch to Protected Mode.
    finit                   ; Initialize the FPU. TODO: This the right way to do it?
    cli                     ; Interrupts off, kernel will take care of initializing them.
    lgdt [gdt_descriptor]   ; Pass the GDT.
    mov eax, cr0
    or eax, 0x1             ; Activate the PM bit in control register 0.
    mov cr0, eax            ; Set the control register for PM.
    
;; PROTECTED MODE STARTS HERE ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

    jmp dword CODE_SEGMENT:setup_pm ; Farjump to clear pipeline, and set CS at the same time.
    
[bits 32]
setup_pm: ; Setup PM once in it.
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax                  ; Assign all data segment registers.
    mov fs, ax                  ; to the new valid GDT segments.
    mov gs, ax
    mov ss, ax
    
    mov ebp, HIGHSTACK          ; Setup the new protected mode stack.
    mov esp, ebp                ; Old one's invalid, oh well.
    
    jmp start_kernel            ; And back to main.
    
start_kernel:
    push dword [HIGHMEM]        ; Push important things onto the stack.
    push dword [LOWMEM]         ; The kernel will confirm there's enough high memory to run, we just need to worry about lomem.
    push dword [BOOTDRIVE]      ; And also be sure to tell it what disk all this is from
    call kernel_main            ; And call kernel_main() in C land.
jmp $                           ; Hang forever on return (OS shutdown).

times 1024-($-$$) db 0          ; Pad out the second stage until the end of the sector.

