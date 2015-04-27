#include "cmos.h"

#include <stddef.h>
#include <stdbool.h>
#include "hardware.h"

int CMOS_read(char reg, bool convert)
{
	int ret;
	
	CMOS_sync();
	
	asm volatile ("cli");
	outb(CMOS_ADDRESS, reg); // Write the register.
	io_wait();
	// TODO: Wait for flag/delay/sleep/something.
	ret = inb(CMOS_DATA); // And read the data.
	asm volatile ("sti"); // TODO: Uncomment when interrupts are mapped.
	
	if (convert) // Convert from BCD if desired.
		ret = ((ret / 16) * 10) + (ret & 0xF);
	
	return ret;
}

void CMOS_write(char reg, char byte)
{
	CMOS_sync();
	
	asm volatile ("cli");
	outb(CMOS_ADDRESS, reg); // First pick a register.
	outb(CMOS_DATA, byte); // Then write the byte.
	asm volatile ("sti"); // TODO: Uncomment when interrupts are mapped.
}

void CMOS_sync()
{
	// TODO: Things will work without this, but really add eventually.
}