#include "irq.h"

#include "debug.h"
#include "hardware.h"
#include "keyboard.h"
#include "timer.h"
#include "pic.h"
#include "string.h"

void process_irq(int irq)
{
	string* errmsg;
	switch(irq)
	{
		case 0: // PIT
			timer_IRQ();
			break;
		case 1: // Keyboard
			keyboard_IRQ();
			break;
		case 6: // Floppy disk
			// TODO: Add when a floppy driver is finally in.
			break;
		case 14: // Primary ATA
			// TODO: Add when an ATA driver is finally in too.
			break;
		default: // Unsupported IRQs:
			errmsg = string_new();
			string_add(errmsg, "Unsupported IRQ detected! IRQ: ");
			string_addnum(errmsg, irq, 10);
			klog(errmsg); // Log the unsupported IRQ.
			break;
	}
}