// Programmable Interrupt Controller functions and setup.

#ifndef KERNEL_PIC
#define KERNEL_PIC

//
// Ports:
//

#define MASTER_PIC_CMD		0x20	// Command port of the primary PIC.
#define MASTER_PIC_DATA		0x21	// Data port of the primary PIC.

#define SLAVE_PIC_CMD		0xA0	// Command port of the secondary PIC.
#define SLAVE_PIC_DATA		0xA1	// Data port of the secondary PIC.

//
// Initialization:
//

// ICW1:
#define PIC_INIT			0x10	// Flag to restart the PIC.
#define PIC_SINGLE			0x2		// Flag to tell the PIC there's only one PIC in the system. (ALMOST NEVER USED)
#define	PIC_LEVEL_TRIGGER	0x8		// Flag to tell the PIC to use Level Triggered mode instead of Edge Triggered.
#define PIC_ICW4			0x1		// Flag to tell the PIC there will be a fourth init word.

// ICW2:
#define MASTER_PIC_OFFSET	0x20	// Vector offset for the first set of IRQs.
#define SLAVE_PIC_OFFSET	0x28	// Vector offset for the second set of IRQs.

// ICW3:
#define	MASTER_CASCADE		4		// Tells the first PIC there's a slave at IRQ2. 	(0000 0100)
#define	SLAVE_CASCADE		2		// Tells the second PIC its slaved identity. 		(0000 0010)

// ICW4:
#define PIC_8086_MODE		0x1		// Tell the PIC to operate in 80x86 mode instead of MCS-80/86.

//
// Commands:
//

#define PIC_EOI				0x20	// End of interrupt signal, sent to the PIC after an IRQ interrupt.
#define PIC_READ_IRR		0x0A	// Command to read the PIC's IRR.
#define PIC_READ_ISR		0x0B	// Command to read the PIC's ISR.

#define IRQ_BEGIN			MASTER_PIC_OFFSET
#define IRQ_END				SLAVE_PIC_OFFSET + 7

void PIC_init();
void send_EOI(unsigned char irq);

#endif