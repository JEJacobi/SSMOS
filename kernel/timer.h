// Programmable Interrupt Timer and system time logic.

#ifndef KERNEL_TIMER
#define KERNEL_TIMER

#define PIT_DIVIDER		1193	// The reload value (or divider) for the PIT.
								// This divider produces a tick every 1.000153 ms.
								// (0000 0100) (1010 1001)
								
#define PIT_DIV_LOW		0xA9	// The lobyte of the divider: (1010 1001)
#define PIT_DIV_HIGH	0x4		// The hibyte of the divider: (0000 0100)
								
#define PIT_COMMAND		0x43	// The PIT command port, write-only.
#define PIT_CHANNEL_0	0x40	// The Channel 0 data port.
#define PIT_CHANNEL_1	0x41	// The Channel 1 data port.
#define PIT_CHANNEL_2	0x42	// The Channel 2 data port.

#define PIT_INIT		0x34	// The initialization byte for the system timer. (0011 0100)
								// (00			11			010			0)
								// Channel 0 	highb/lowb 	rate gen	binary mode

void timer_init();				// Initialize the PIT.
void timer_IRQ();				// PIT IRQ handler.
unsigned long get_ticks();		// Return the total number of ticks since system start.

void ksleep(int milliseconds);	// Do nothing, intentionally.

#endif