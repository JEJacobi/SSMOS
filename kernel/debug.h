// Kernel error handling systems.
#include "list.h"
#include "string.h"

#ifndef KERNEL_DEBUG
#define KERNEL_DEBUG

#define HALTDUMP_FG			WHITE
#define HALTDUMP_BG			RED

void haltdump(char* msg, int err);	// Basically a BSOD, stop the OS and display data.
void klog(char* msg);				// Log a kernel message (error, warning, whatever) in the system log.
struct element* getlog();			// Returns a pointer to the system log list.			

#endif