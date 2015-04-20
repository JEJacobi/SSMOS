// Kernel error handling systems.

#ifndef KERNEL_DEBUG
#define KERNEL_DEBUG

void haltdump(char* msg);				// Basically a BSOD, stop the OS and display data.
void klog(char* msg);					// Log a kernel message (error, warning, whatever) in the system log.

#endif