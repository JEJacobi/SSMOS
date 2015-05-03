// Floppy drive controller driver and logic.

#ifndef KERNEL_FLOPPY
#define KERNEL_FLOPPY

#include <stdbool.h>

#define FLOPPY_CMOS_REGISTER	0x10	// The CMOS register to read floppy disk info from.

// Floppy Drive Types:
#define	FLOPPY_NO_DRIVE			0x0		// No floppy drive in specified port.
#define FLOPPY_360KB_5_25		0x1		// A 360 KB 5.25" floppy drive.
#define FLOPPY_1_2MB_5_25		0x2		// A 1.2 MB 5.25" floppy drive.
#define FLOPPY_720KB_3_5		0x3		// A 720 KB 3.5" floppy drive.
#define FLOPPY_1_44MB_3_5		0x4		// A 1.44 MB 3.5" floppy drive.
#define FLOPPY_2_88MB_3_5		0x5		// A 2.88 MB 3.5" floppy drive.

int floppy_gettype(bool slave);			// Return the floppy drive type as shown by the CMOS register.
char* floppy_getstring(bool slave);		// Return a description string of the floppy drive.

#endif