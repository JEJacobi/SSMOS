#include "debug.h"

#include "graphics.h"
#include "output.h"
#include "list.h"
#include "signal.h"
#include "string.h"

struct element* systemlog = NULL;

void haltdump(char* msg, int err)
{
    int eax, ebx, ecx, edx, esi, edi;
    int* stack;
    
    asm volatile ("" // Load variables with initial register states.
                :"=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx), "=S"(esi), "=D"(edi));
                
    asm volatile ("movl %%ebp, %%eax" : "=a"(stack));
    
    char dumpcolor = get_color(HALTDUMP_FG, HALTDUMP_BG); // Get the color to use for printing.
    void* eip = __builtin_return_address(1); // Get the return address of the function above this one (as EIP).
    
    asm volatile("cli"); // Disable interrupts, no reason to keep them running.
    
    kclear(); // Clear the screen and fill with color.
    kccolor(get_position(0, 0), COLUMNS * ROWS * 2, dumpcolor);
    
    kprint(get_position(1, 1), "!! FATAL PROBLEM - KERNEL HALTDUMP !!", dumpcolor);
    
    kprint(get_position(1, 3), "Message:", dumpcolor);
    kprint(get_position(10, 3), msg, dumpcolor);
    
    kprint(get_position(1, 4), "Error #:", dumpcolor);
    kprintnum(get_position(10, 4), err, dumpcolor, 10);
    
    kprint(get_position(1, 6), "Caller Location: 0x", dumpcolor);
    kprintnum(get_position(20, 6), (int)eip, dumpcolor, 16);
    
    // Print the stack:
    kprint(get_position(1, 9), "Stack: 0x", dumpcolor);
    kprintnum(get_position(10, 9), (int)stack, dumpcolor, 16);
    
    kprintnum(get_position(1, 11), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(13, 11), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(25, 11), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(37, 11), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(49, 11), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(61, 11), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(1, 12), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(13, 12), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(25, 12), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(37, 12), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(49, 12), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(61, 12), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(1, 13), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(13, 13), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(25, 13), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(37, 13), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(49, 13), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(61, 13), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(1, 14), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(13, 14), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(25, 14), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(37, 14), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(49, 14), *stack, dumpcolor, 16); stack++;
    kprintnum(get_position(61, 14), *stack, dumpcolor, 16); stack++;
    
    // Print the registers:
    kprint(get_position(1, 16), "Registers:", dumpcolor);
    
    kprint(get_position(1, 18), "EAX:", dumpcolor);
    kprintnum(get_position(6, 18), eax, dumpcolor, 16);
    
    kprint(get_position(24, 18), "EBX:", dumpcolor);
    kprintnum(get_position(29, 18), ebx, dumpcolor, 16);
    
    kprint(get_position(47, 18), "ECX:", dumpcolor);
    kprintnum(get_position(52, 18), ecx, dumpcolor, 16);
    
    kprint(get_position(1, 19), "EDX:", dumpcolor);
    kprintnum(get_position(6, 19), edx, dumpcolor, 16);
    
    kprint(get_position(24, 19), "ESI:", dumpcolor);
    kprintnum(get_position(29, 19), esi, dumpcolor, 16);
    
    kprint(get_position(47, 19), "EDI:", dumpcolor);
    kprintnum(get_position(52, 19), edi, dumpcolor, 16);
    
    kprint(get_position(1, 23), "If this problem has occurred multiple times, please report it to a developer.", dumpcolor);
    
    kflip();
    while(true) { } // And loop forever, no recovering from a haltdump.
}

void klog(char* msg)
{
    if (systemlog == NULL)
        systemlog = list_new(msg);
    else
        list_add(systemlog, msg);
}

struct element* getlog()
{
    return systemlog;
}