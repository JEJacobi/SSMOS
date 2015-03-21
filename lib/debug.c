#include "debug.h"

#include <stdbool.h>
#include "system.h"

void assert(bool exp)
{
	if(!exp)
		haltdump();
}

void haltdump()
{
	//TODO: Will need system calls working, print registers and memory to the screen.

	while(true) { } // Do nothing, forever.
}