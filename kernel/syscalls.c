#include "syscalls.h"

#include <stddef.h>

#include "debug.h"
#include "memory.h"
#include "graphics.h"
#include "terminal.h"
#include "output.h"

int process_syscall(int eax, int ebx, int ecx, int edx, int esi)
{
	switch(eax)
	{
		// sleep
		case 0x0: 
			break;
		
		// system
		case 0x1:
			break;
		
		// clear
		case 0x2:
			clear();
			break; 
		
		// malloc
		case 0x3: return malloc(ebx); 
		
		// realloc
		case 0x4: return realloc(ebx, ecx);
			
		// free	
		case 0x5: free(ebx);
			break; 
		
		// memcpy
		case 0x6: memcpy(ebx, ecx, edx);
			break;
			
		// memset
		case 0x7: memset(ebx, ecx, edx);
			break;
		
		// putchar
		case 0x8:
			break;
		
		// putstring
		case 0x9:
			break;
			
		// print
		case 0xA:
			break;
			
		// printnum
		case 0xB:
			break;
			
		// ccolor
		case 0xC:
			break;
		
		// scroll
		case 0xD: scroll();
			break;
		
		// flip
		case 0xE: flip();
			break;
		
		// sync
		case 0xF: sync();
			break;
			
		default: // unknown or error
			printnum( // TEMP
				get_position(0, 0),
				eax,
				get_color(LIGHT_GREEN, BLACK),
				16);
			flip();
			haltdump("Unknown or invalid system call.");
	}
	return NULL;
}