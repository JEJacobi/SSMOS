#include "irq.h"

#include "debug.h"
#include "hardware.h"
#include "keyboard.h"
#include "timer.h"
#include "pic.h"
#include "string.h"

void process_irq(int irq)
{
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
		default: // Unsupported IRQs:
			break;
	}
}
