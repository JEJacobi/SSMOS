// Methods for interacting with the CMOS and its registers.

#ifndef LIB_CMOS
#define LIB_CMOS

#include <stdbool.h>

// General Registers:
#define CMOS_ADDRESS	0x70
#define CMOS_DATA		0x71

#define	CMOS_STATUS_A	0xA
#define CMOS_STATUS_B	0xB

// Floppy Size Register:
#define CMOS_FLOPPY		0x10

// RTC Registers:
#define	CMOS_SECONDS	0x0
#define	CMOS_MINUTES	0x2
#define CMOS_HOURS		0x4
#define	CMOS_WEEKDAY	0x6
#define CMOS_DAYOFMONTH	0x7
#define CMOS_MONTHS		0x8
#define CMOS_YEARS		0x9
#define CMOS_CENTURY	0x32	// NOT GUARANTEED

int CMOS_read(char reg, bool convert);	// Read a byte from a CMOS register.
void CMOS_write(char reg, char byte);	// Write a byte to a CMOS register.
void CMOS_sync();						// Wait for there to be no RTC updates or any flags.

#endif