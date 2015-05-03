// OS-defined return signals:

#ifndef LIB_SIGNAL
#define LIB_SIGNAL

// STANDARD:
#define SIG_SUCCESS			0	// Succeed, with no errors.
#define SIG_FAIL			-1	// Fail, with no errors.

// ERROR:
#define ERR_CANNOT_FIND		1	// Cannot find file or directory.
#define ERR_OUT_OF_MEMORY	2	// Out of memory.
#define ERR_READ_FAULT		3	// Cannot read from disk.
#define ERR_WRITE_FAULT		4	// Cannot write to disk.
#define ERR_STACK_OVERFLOW	5	// The stack has overflowed.
#define ERR_GPF				6	// Something has triggered a GPF
#define	ERR_NO_DEVICE		7	// The device does not exist.
#define ERR_INVALID_DATA	8	// The data is invalid.
#define ERR_INVALID_NAME	9	// The name is invalid.
#define ERR_FILE_TOO_BIG	10	// File is too big to access.
#define ERR_IO				11	// General IO Error.
#define ERR_HARDWARE		12	// General hardware error, out of the kernel's control.

#endif