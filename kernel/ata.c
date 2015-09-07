#include "ata.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "hardware.h"
#include "timer.h"

uint16_t identify[IDENTIFY_SIZE];   // The buffer for reading IDENTIFY returns. Is always 256 16-bit values.
ATA_DRIVE primary_selected;         // Which drive is currently selected on the primary bus.
ATA_DRIVE secondary_selected;       // Which drive is currently selected on the secondary bus.

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
    
    if (inb(port) == FLOATING_BUS ||
        inb(port) == 0x0) // Read the status byte and check for a floating bus (no devices attached).
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

bool ata_identify(ATA_BUS bus, ATA_DRIVE drive)
{
    int i;
    short data;
    short clear;
    short cmd;
    
    if (bus == PRIMARY) // Port will be used for clearing values.
    {
        data = PRIMARY_DATA;
        clear = PRIMARY_SECTOR;
        cmd = PRIMARY_CMD_STATUS;
    }
    else
    {
        data = SECONDARY_DATA;
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
        return false; // Return if we've just sent IDENTIFY to a non-existing drive.
    
    ata_poll_bsy(bus);
    
    if (inb(clear) != 0x0) // Check LBA high to be non-zero, and return if it is.
        return false;
    clear--;
    if (inb(clear) != 0x0) // Check LBA mid to be non-zero, and return if it is.
        return false;
        
    if (ata_poll_drq(bus) == false) // Poll DRQ until either it or ERR sets.
        return false; // TEMP, should actually add some error handling at some point.
        
    for (i = 0; i < IDENTIFY_SIZE; i++)
        identify[i] = inw(data); // Read 256 words from the data port.
    return true;
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
    
    if (lba_mid == 0x0 && lba_high == 0x0)
        return ATA;
    if (lba_mid == 0x3C && lba_high == 0xC3)
        return SATA;
    if (lba_mid == 0x14 && lba_high == 0xEB)
        return ATAPI;
    if (lba_mid == 0x69 && lba_high == 0x96)
        return SATAPI;
    
    return NONE;
}

void ata_reset(ATA_BUS bus)
{
    short port;
    
    if (ata_floating(bus) == true)
        return; // If the bus is floating, nothing to reset.
    
    if (bus == PRIMARY)
    {
        port = PRIMARY_DCR;
        primary_selected = MASTER; // On software reset, the master is automatically selected.
    }
    else
    {
        port = SECONDARY_DCR;
        secondary_selected = MASTER; // Same for the secondary.
    }
    
    outb(port, 0x4);    // (0000 0100) - 2nd bit is the software reset flag.
    outb(port, 0x0);    // And once the drive is reset, clear the DCR manually.
    ata_delay(bus);     // Create a little bit of delay for the status registers to update.
    
    // Loop until BSY is gone and RDY is set.
    while ( check_bit(inb(port), STATUS_RDY) == false 
            || check_bit(inb(port), STATUS_BSY) == true) { }
}

void ata_irq(ATA_BUS bus)
{
    // Do I even need to use this at all?
    // Maybe just totally disable IRQs in init.
}

bool ata_poll_drq(ATA_BUS bus)
{
    short reg;
    char status;
    
    if (bus == PRIMARY)
        reg = PRIMARY_CMD_STATUS;
    else
        reg = SECONDARY_CMD_STATUS;
    
    do
    {
        status = inb(reg);
        
        if (check_bit(status, STATUS_ERR) == true)
            return false; // If ERR has set, something's gone wrong, return false.
    } while (check_bit(status, STATUS_DRQ) == false);

    if (check_bit(status, STATUS_DRQ) == true &&
        check_bit(status, STATUS_ERR) == false)
        return true; // Make doubly sure ERR hasn't set, but DRQ has.
    else
        return false;
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

char* ata_tostring(ATA_BUS bus, ATA_DRIVE drive)
{
    switch (ata_type(bus, drive))
    {
        case ATA:
            return "ATA";
        case ATAPI:
            return "ATAPI";
        case SATA:
            return "SATA";
        case SATAPI:
            return "SATAPI";
        case NONE:
            return "NONE/UNSUPPORTED";
        default:
            return "ERROR!";
    }
}
