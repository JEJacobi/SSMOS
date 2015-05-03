#include "floppy.h"

#include <stdbool.h>
#include "debug.h"
#include "cmos.h"
#include "hardware.h"

int floppy_gettype(bool slave)
{
	// First, get the actual value from CMOS.
	int reg = CMOS_read(FLOPPY_CMOS_REGISTER, false);
	
	if (!slave) // If we want the master floppy, use the high nibble.
	{
		return reg >> 4; // Just shift the upper four bits down.
	}
	else // If we want the slave floppy, use the low nibble.
	{
		return reg & 0xF; // Mask the upper four bits.
	}
}

char* floppy_getstring(bool slave)
{
	int type = floppy_gettype(slave);
	
	switch(type)
	{
		case FLOPPY_NO_DRIVE:
			return "NONE";
		case FLOPPY_360KB_5_25:
			return "360KB 5.25\"";
		case FLOPPY_1_2MB_5_25:
			return "1.2MB 5.25\"";
		case FLOPPY_720KB_3_5:
			return "720KB 3.5\"";
		case FLOPPY_1_44MB_3_5:
			return "1.44MB 3.5\"";
		case FLOPPY_2_88MB_3_5:
			return "2.88MB 3.5\"";
		default:
			haltdump("Unknown floppy drive type!");
	}
	return NULL; // This won't happen.
}