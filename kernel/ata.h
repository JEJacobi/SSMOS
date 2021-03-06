#ifndef KERNEL_ATA
#define KERNEL_ATA

#include <stdbool.h>

#define PRIMARY_ATA         0x1F0               // IO origin of the primary ATA bus.
#define PRIMARY_DCR         0x3F6               // Location of the primary device control register.

#define PRIMARY_DATA        PRIMARY_ATA + 0     // Port to read and write PIO data byte through.
#define PRIMARY_ERROR       PRIMARY_ATA + 1     // Port for features and error info, probably not used here.
#define PRIMARY_SECTOR      PRIMARY_ATA + 2     // Number of sectors to read or write.
#define PRIMARY_LBA_LOW     PRIMARY_ATA + 3     // This and the following two are LBA specific addresses.
#define PRIMARY_LBA_MID     PRIMARY_ATA + 4
#define PRIMARY_LBA_HIGH    PRIMARY_ATA + 5
#define PRIMARY_DRIVE_HEAD  PRIMARY_ATA + 6     // Used to select a drive or head. 
#define PRIMARY_CMD_STATUS  PRIMARY_ATA + 7     // Used to send commands or read the drive status.

//////////////////////////////////////////////////

#define SECONDARY_ATA       0x170               // IO origin of the secondary ATA bus.
#define SECONDARY_DCR       0x376               // Location of the secondary device control register.
                                                // Secondary IO offsets mimic primary, see above comments for details.

#define SECONDARY_DATA          SECONDARY_ATA + 0
#define SECONDARY_ERROR         SECONDARY_ATA + 1
#define SECONDARY_SECTOR        SECONDARY_ATA + 2
#define SECONDARY_LBA_LOW       SECONDARY_ATA + 3
#define SECONDARY_LBA_MID       SECONDARY_ATA + 4
#define SECONDARY_LBA_HIGH      SECONDARY_ATA + 5
#define SECONDARY_DRIVE_HEAD    SECONDARY_ATA + 6
#define SECONDARY_CMD_STATUS    SECONDARY_ATA + 7

//////////////////////////////////////////////////

// STATUS BYTE:

#define STATUS_ERR          0x0                 // Error bit. Something's gone wrong if this is set.
#define STATUS_DRQ          0x3                 // DRQ bit, set when ready to accept data or the drive has some to send.
#define STATUS_SRV          0x4                 // Overlapped mode service request bit.
#define STATUS_DF           0x5                 // Drive fault error bit, does not implicitly set STATUS_ERR.
#define STATUS_RDY          0x6                 // Should always be set in normal operations. Off when spun down or error.
#define STATUS_BSY          0x7                 // Drive is busy preparing to do something. Always wait for it to clear.


// DEVICE CONTROL REGISTER / ALTERNATE STATUS:

#define DCR_NIEN            0x1                 // Set this bit to stop the selected device from outputting interrupts.
#define DCR_SRST            0x2                 // Set this to do a software reset of ALL ATA drives, in case of error.
#define DCR_HOB             0x7                 // Set to read back the High Order Byte of the last LBA value. Probably off.


// COMMANDS:

#define CACHE_FLUSH         0xE7                // Write to the command register to tell the ATA drive to flush its cache.
#define READ_MULTIPLE       0xC4                // Command to read multiple blocks of sectors, instead of a single one.
#define WRITE_MULTIPLE      0xC5                // Command to write to multiple blocks of sectors, instead of a single one.
#define IDENTIFY            0xEC                // Command to signal the ATA bus to identify itself.
#define DEVICE_DIAGNOSTICS  0x90                // Command to signal device diagnostics to execute.
#define SELECT_MASTER       0xA0                // Flag to send to the IDE to select the master drive on the bus.
#define SELECT_SLAVE        0xB0                // Flag to send to the IDE to select the slave drive on the bus.


// MISC:

#define IDENTIFY_SIZE       256                 // Size of the IDENTIFY return/buffer.

#define TIMEOUT             20000               // Command timeout threshold, in milliseconds.

#define FLOATING_BUS        0xFF                // If this is read from the regular status byte, the bus has no drives.

typedef enum { PRIMARY, SECONDARY } ATA_BUS;
typedef enum { MASTER, SLAVE } ATA_DRIVE;
typedef enum { ATA, ATAPI, SATA, SATAPI, NONE } ATA_TYPE;

void ata_select(ATA_BUS bus, ATA_DRIVE drive);  // Selects drive for operations on the provided bus.
void ata_delay(ATA_BUS bus);                    // Creates the standard 500ns delay by reading status port data from bus.
void ata_flush(ATA_BUS bus);                    // Issue a cache flush command to the specified bus.
bool ata_floating(ATA_BUS bus);                 // Check if the bus is floating (no drives connected). Returns true if it is.
bool ata_exists(ATA_BUS bus, ATA_DRIVE drive);  // See if drive primary/secondary : master/slave exists. WILL OVERWRITE identify[].
bool ata_identify(ATA_BUS bus, ATA_DRIVE drive);// IDENTIFY the target drive and load it into the identify[] buffer.
ATA_TYPE ata_type(ATA_BUS bus, ATA_DRIVE drive);// Return the type of drive at the specified location. Returns NONE if no drive.
void ata_reset(ATA_BUS bus);                    // Reset all ATA drives on the specified bus.
void ata_irq(ATA_BUS bus);                      // ATA IRQ handler. Does nothing right now.
bool ata_poll_drq(ATA_BUS bus);                 // Waits for DRQ to unset, returns true if successful. Returns false if ERR sets.
void ata_poll_bsy(ATA_BUS bus);                 // Poll until BSY goes away. Probably add a timeout at some point.
char* ata_tostring(ATA_BUS bus, ATA_DRIVE drive);// Return a string containing the ATA type.

// Return values are generally used as error checking here.

#endif

