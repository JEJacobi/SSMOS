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
		case 0x2: kclear();
			break; 
		
		// malloc
		case 0x3: return (int)kmalloc((size_t)ebx); 
		
		// realloc
		case 0x4: return (int)krealloc((void*)ebx, (size_t)ecx);
			
		// free	
		case 0x5: kfree((void*)ebx);
			break; 
		
		// memcpy
		case 0x6: kmemcpy((void*)ebx, (void*)ecx, (size_t)edx);
			break;
			
		// memset
		case 0x7: kmemset((void*)ebx, ecx, (size_t)edx);
			break;
		
		// putchar
		case 0x8: kputchar(ebx, ecx, edx);
			break;
		
		// putstring
		case 0x9: kputstring(ebx, (char*)ecx, edx, esi);
			break;
			
		// print
		case 0xA: kprint(ebx, (char*)ecx, edx);
			break;
			
		// printnum
		case 0xB: kprintnum(ebx, ecx, edx, esi);
			break;
			
		// ccolor
		case 0xC: kccolor(ebx, ecx, edx);
			break;
		
		// scroll
		case 0xD: kscroll();
			break;
		
		// flip
		case 0xE: kflip();
			break;
		
		// sync
		case 0xF: ksync();
			break;
			
		default: // unknown or error
			kprintnum( // TEMP
				get_position(0, 0),
				eax,
				get_color(LIGHT_GREEN, BLACK),
				16);
			kflip();
			haltdump("Unknown or invalid system call.");
	}
	return (int)NULL;
}