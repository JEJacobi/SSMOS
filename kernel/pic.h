// Programmable Interrupt Controller functions and setup.

#ifndef KERNEL_PIC
#define KERNEL_PIC

#define MASTER_PIC_CMD		0x20	// Command port of the primary PIC.
#define MASTER_PIC_DATA		0x21	// Data port of the primary PIC.

#define SLAVE_PIC_CMD		0xA0	// Command port of the secondary PIC.
#define SLAVE_PIC_DATA		0xA1	// Data port of the secondary PIC.

#define MASTER_PIC_OFFSET	0x20	// Vector offset for the first set of IRQs.
#define SLAVE_PIC_OFFSET	0x28	// Vector offset for the second set of IRQs.

#define PIC_EOI				0x20	// End of interrupt signal, sent to the PIC after an IRQ interrupt.

void init_PIC();
void send_EOI(unsigned char irq);

#endif