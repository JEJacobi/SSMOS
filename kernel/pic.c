#include "pic.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "hardware.h"

void init_PIC()
{
	
}

void send_EOI(unsigned char irq)
{
	if(irq >= 8) // Send the EOI to the slave if needed.
		outb(SLAVE_PIC_CMD, PIC_EOI);
	
	outb(MASTER_PIC_CMD, PIC_EOI); // But always send an EOI to the master PIC.
}