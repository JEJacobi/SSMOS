#include "ata.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "hardware.h"

uint16_t identify[256]; // The buffer for reading IDENTIFY returns. Is always 256 16-bit values.
ATA_DRIVE primary_selected; // Which drive is currently selected on the primary bus.
ATA_DRIVE secondary_selected; // Which drive is currently selected on the secondary bus.

void ata_select(ATA_BUS bus, ATA_DRIVE drive)
{
	char value;
	
	if (drive == MASTER) // Convert the enum into the drive select command specified.
		value = SELECT_MASTER;
	else
		value = SELECT_SLAVE;
	
	ata_delay(bus); // This might not need to be here. Just in case, though.
	if (bus == PRIMARY)
	{
		if (primary_selected == drive)
			return; // Return if the drive is already selected.
		
		outb(PRIMARY_DRIVE_HEAD, value); // Use the bus specified, and send the drive change command.
		primary_selected = drive;
	}
	else
	{
		if (secondary_selected == drive)
			return; // Return if the drive is already selected.
		
		outb(SECONDARY_DRIVE_HEAD, value);
		secondary_selected = drive;
	}
	ata_delay(bus); // And delay another few hundred nanoseconds before returning, to give the drive time to update.
}

void ata_delay(ATA_BUS bus)
{
	short port;
	
	if (bus == PRIMARY) // Set which bus to read bogus status registers from.
		port = PRIMARY_DCR;
	else
		port = SECONDARY_DCR;
	
	inb(port); // And read five times to create a ~500ns delay.
	inb(port);
	inb(port);
	inb(port);
	inb(port);
}

void ata_flush(ATA_BUS bus)
{
	short port;
	
	if (bus == PRIMARY) // Select which port to send the command to, as usual.
		port = PRIMARY_CMD_STATUS;
	else
		port = SECONDARY_CMD_STATUS;
	
	outb(port, CACHE_FLUSH); // Write the CACHE_FLUSH command byte to the selected port.
	ata_poll_bsy(bus); // And poll until BSY disappears.
}

bool ata_floating(ATA_BUS bus)
{
	short port;
	
	if (bus == PRIMARY)
		port = PRIMARY_CMD_STATUS;
	else
		port = SECONDARY_CMD_STATUS;
	
	if (inb(port) == FLOATING_BUS) // Read the status byte and check for a floating bus (no devices attached).
		return true;
	else
		return false;
}

bool ata_exists(ATA_BUS bus, ATA_DRIVE drive)
{
	if (ata_floating(bus) == true)
		return false; // If the bus is floating, there are no drives attached at all, no need to continue.
	
	// TODO: Sort out individual drives. Somehow.
	return false; // TEMP
}

void ata_identify(ATA_BUS bus, ATA_DRIVE drive)
{
	short clear;
	short cmd;
	
	if (bus == PRIMARY) // Port will be used for clearing values.
	{
		clear = PRIMARY_SECTOR;
		cmd = PRIMARY_CMD_STATUS;
	}
	else
	{
		clear = SECONDARY_SECTOR;
		cmd = SECONDARY_CMD_STATUS;
	}
	
	ata_select(bus, drive);
	
	outb(clear, 0x0); // Clear the sector count.
	clear++;
	outb(clear, 0x0); // Clear LBA low.
	clear++;
	outb(clear, 0x0); // Clear LBA mid.
	clear++;
	outb(clear, 0x0); // Clear LBA high.
	
	outb(cmd, IDENTIFY); // Send the identify command to the selected drive on the provided bus.
	if (inb(cmd) == 0x0)
		return; // Return if we've just sent IDENTIFY to a non-existing drive.
	
	ata_poll_bsy(bus);
	
	if (inb(clear) != 0x0) // Check LBA high to be non-zero, and return if it is.
		return;
	clear--;
	if (inb(clear) != 0x0) // Check LBA mid to be non-zero, and return if it is.
		return;
		
	// TODO: Poll until DRQ or ERR sets, and read data into IDENTIFY buffer.
}

ATA_TYPE ata_type(ATA_BUS bus, ATA_DRIVE drive)
{
	unsigned char lba_mid;
	unsigned char lba_high;
	short port;
	
	if (bus == PRIMARY)
		port = PRIMARY_LBA_MID; // Select appropriate LBA mid port.
	else
		port = SECONDARY_LBA_HIGH;
	
	ata_reset(bus); // Reset the bus, and select working drive.
	ata_select(bus, drive);
	
	lba_mid = inb(port); // Read signature byte from LBA_mid.
	lba_high = inb(port + 1); // And read the other byte from LBA_high.
	
	// TODO: Signature byte differentiating magic goes here. After I figure out what it does.
	
	return NONE;
}

void ata_reset(ATA_BUS bus)
{
	// TODO: Set bit two on the bus control register, wait a bit, and clear it.
}

void ata_irq(ATA_BUS bus)
{
	// Do I even need to use this at all?
	// Maybe just totally disable IRQs in init.
}

void ata_poll_bsy(ATA_BUS bus)
{
	short reg;
	char status;
	
	if (bus == PRIMARY) // Select the status register to read from (primary or secondary).
		reg = PRIMARY_CMD_STATUS;
	else
		reg = SECONDARY_CMD_STATUS;
	
	do // Poll the status register and keep checking BSY until it goes away.
	{
		status = inb(reg);
	} while (check_bit(status, STATUS_BSY) == true);
}
