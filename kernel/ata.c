#include "ata.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "hardware.h"

uint16_t identify[256]; // The buffer for reading IDENTIFY returns. Is always 256 16-bit values.

bool ata_floating(bool secondary)
{
	if (!secondary) // Check the primary bus.
	{
		if(inb(PRIMARY_CMD_STATUS) == FLOATING_BUS) // Check if reading a byte returns an invalid value.
			return true;							// If so, it's definitely floating.
		else
			return false;
	}
	else // Check the secondary bus.
	{
		if(inb(SECONDARY_CMD_STATUS) == FLOATING_BUS)
			return true;
		else
			return false;
	}
}

bool ata_exists(bool slave, bool secondary)
{
	if (ata_floating(secondary) == true)
		return false; // If the bus (not even the drive) is floating, there's definitely no drive.

	// TODO: Actually make sure the disk isn't there too.
	return true;
}

void ata_identify(bool slave, bool secondary)
{

}

void ata_reset(bool secondary)
{

}

