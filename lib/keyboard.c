#include "keyboard.h"

#include <stdbool.h>
#include "hardware.h"

volatile enum scancode curr_scancode;   // Current scancode.
volatile enum scancode last_scancode;   // Last scancode.

void keyboard_IRQ()
{
    // TODO: Rewrite into interrupt-based driver.
}

enum scancode poll_scancode()
{
    // TODO: Call from poll key
}

char poll_key() // Slow and pretty inefficient. Temporary non-interrupt based workaround.
{
    while(!get_output_status()) { } // Wait until the input bit is flagged.
    
    last_scancode = curr_scancode;
    curr_scancode = inb(KBD_DATA_PORT);
    // Now that input is confirmed, get scancode.
    
    // Handle shift-keys
    
    return scan_to_key(curr_scancode);
}

void pause()
{
    poll_key(); //TODO: Replace with some kind of release scancode detector?
}

char get_status()
{
    return inb(KBD_CMD_PORT); // Return the status register from the command port.
}

char scan_to_key(enum scancode sc)
{
    if (sc > 0x80)
        return '\0';
    
    static char scan_lower[]  = "\0\r1234567890-=\b\tqwertyuiop[]\n\vasdfghjkl;\'`\f\0zxcvbnm,./\f*\0 ";
    static char scan_higher[] = "";

    return scan_lower[(int)sc];
}

bool get_output_status()
{
    return check_bit(get_status(), 0);
    // Check if bit 0 (output buffer status) from the status register is set.
}

bool get_input_status()
{
    return check_bit(get_status(), 1);
    // Check if bit 1 (input buffer status) from the status register is set.
}