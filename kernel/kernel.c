////////////////////
// SSMOS - KERNEL //
////////////////////

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "array.h"
#include "cmos.h"
#include "pic.h"
#include "timer.h"
#include "debug.h"
#include "list.h"
#include "hardware.h"
#include "string.h"
#include "math.h"
#include "memory.h"
#include "graphics.h"
#include "interrupts.h"
#include "terminal.h"
#include "output.h"

#define BOOT_FOREGROUND			LIGHT_GREEN
#define BOOT_BACKGROUND			BLACK
#define SHUTDOWN_FG				WHITE
#define SHUTDOWN_BG				BLUE
#define MSGS_OFFSET				8
#define DATA_OFFSET				31
#define DATA_BASE				10

#define STACK_TOP				0x7C00
#define STACK_BOTTOM			0x500

char *logo =    "   _____    _____   __  __    ____     _____ \n"
				"  / ____|  / ____| |  \\/  |  / __ \\   / ____|\n"
				" | (___   | (___   | \\  / | | |  | | | (___  \n"
				"  \\___ \\   \\___ \\  | |\\/| | | |  | |  \\___ \\ \n"
				"  ____) |  ____) | | |  | | | |__| |  ____) |\n"
				" |_____/  |_____/  |_|  |_|  \\____/  |_____/ \n\n";
				
char *msgs = 	" Booting from drive number   :\n"
				" Low memory detected   (KiB) :\n"
				" High memory detected  (KiB) :\n"
				" Current stack size    (KiB) :\n"
				" Entering terminal mode...    \n";
				
char *shtdown =	" It is now safe to turn off the computer.                                       ";

void kernel_main(int bdrive, int lomem, int himem)
{	
	interrupts_init();	// Initialize the interrupts, and IDT.
	memory_init(himem); // Initialize the heap and memory handlers.
	graphics_init(); 	// Initialize the display stuff.
	PIC_init();			// Initialize the 8259 PIC.
	timer_init();		// Initialize the 8253/8254 PIT and system timer.
	
	enable_interrupts(); // And turn the interrupts back on after the bootloader disable.
	
	//haltdump("TEST!", 0); // TEMP
	
	kclear();
	
	// Get the color to be used for printing, pre terminal start.
	char boot_color = get_color(BOOT_FOREGROUND, BOOT_BACKGROUND);
	
	kprint( // Print the SSMOS logo.
		get_position(0, 0),
		logo,
		boot_color);
		
	kprint( // Print the detection messages (bootdrive, lowmem, himem).
		get_position(0, MSGS_OFFSET),
		msgs,
		boot_color);
		
	kprintnum( // Print the bootdrive.
		get_position(DATA_OFFSET, MSGS_OFFSET),
		bdrive,
		boot_color,
		DATA_BASE);
		
	kprintnum( // Print the low memory.
		get_position(DATA_OFFSET, MSGS_OFFSET + 1),
		lomem,
		boot_color,
		DATA_BASE);
		
	kprintnum( // Print the high memory.
		get_position(DATA_OFFSET, MSGS_OFFSET + 2),
		himem,
		boot_color,
		DATA_BASE);
		
	kprintnum( // Print stack size.
		get_position(DATA_OFFSET, MSGS_OFFSET + 3),
		(STACK_TOP - STACK_BOTTOM) / 1024,
		boot_color,
		DATA_BASE);
	
	kflip();
	
	// Initialize the terminal.
	init_terminal();
	
	// And transfer to it. All userspace programs will run on this,
	// so we'll only return on shutdown.
	run_terminal();
	
	// On return from terminal, prepare the system for shutdown.
	
	disable_interrupts(); // Stop any interrupts.
	
	kprint( // Print a shutdown message on the last row.
		get_position(0, ROWS - 1),
		shtdown,
		get_color(SHUTDOWN_FG, SHUTDOWN_BG));
	
	kflip(); // Flip the buffer for the last time.
	
	return; // And return, where we'll loop forever.
}
