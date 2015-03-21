#ifndef LIB_DEBUG
#define LIB_DEBUG

#include <stdbool.h>

void assert(bool exp);			// Asserts that exp is true. If not, stop the OS and display data.
void haltdump();				// Basically a BSOD, stop the OS and display data.

#endif