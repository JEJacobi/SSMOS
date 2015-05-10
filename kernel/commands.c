#include "commands.h"

#include <stdbool.h>
#include <stddef.h>

#include "ata.h"
#include "cmos.h"
#include "date.h"
#include "debug.h"
#include "memory.h"
#include "keyboard.h"
#include "interrupts.h"
#include "hardware.h"
#include "floppy.h"
#include "terminal.h"
#include "timer.h"
#include "graphics.h"
#include "output.h"
#include "list.h"
#include "string.h"
#include "signal.h"
#include "version.h"

#include "system.h" // TEMP

struct command commands[] = // List of built in command structs, their ID's, help strings, and function pointers:
	{
		{ "help", "Get a list of commands, or more specific information like you're doing now.",
		"help (x) - x: given command", &help },
		
		{ "history", "Display recent terminal history.", "history - no parameters", &history },
		
		{ "disk", "Display disk drive information or change working disk.", "disk (x) - x: disk to change to (fd1/fd2/hd1/hd2)" , &disk},
		
		{ "memview", "Display a screen of memory, starting at the specified location.", 
		"memview (x) - x: starting location", &memview },
		
		{ "echo", "Echo a string onto a new line.", "echo (x) - x: string to echo", &echo},
		
		{ "cls", "Clears the screen.", "cls - no parameters", &cls },
		
		{ "tcolor", "Changes the text and screen color. Valid colors are: black, blue, cyan, green,  magenta, red, brown, and white.", "tcolor (x) (y) - x: foreground, y: background", &tcolor },
		
		{ "alias", "Aliases parameters as a command, however normal commands take precedence.",
		"alias (x) (y) - x: alias name, y: command", &alias },
		
		{ "cprompt", "Changes the terminal prompt to the string entered.", "cprompt (x) - x: new prompt", &cprompt },
		
		{ "cpuinfo", "Display known CPU information, features, and instruction sets.", "hardware - no parameters", &cpuinfo},
		
		{ "syslog", "Displays the recent system log.", "syslog - no parameters", &syslog },
		
		{ "memory", "Displays operating system memory information.", "memory - no parameters", &memory },
		
		{ "reboot", "Reboots the computer.", "reboot - no parameters", &reboot },
		
		{ "shutdown", "Stop all tasks and prepare the OS for shutdown.", "shutdown - no parameters", &shutdown },
		
		{ "time", "Display uptime, as well as the real time.", "time - no parameters", & time },
		
		{ "debug", "Used to trigger an external debugger.", "debug - no parameters", &debug },
		
		{ "about", "The about page for SSMOS.", "about - no parameters", &about },
		
		{ "version", "Displays the current version of SSMOS.", "version - no parameters", &version }
	};
	
struct element* aliases;
	
int num_commands = (sizeof commands / sizeof(struct command));

struct command* find_cmd(char* input)
{
	int i;
	for (i = 0; i < num_commands; i++)
	{
		if (strcmp(commands[i].name, input) == 0) // Find a string that's equal to input.
			return &commands[i]; // If found, return a pointer to the command.
	}
	return NULL; // If none are found, return NULL.
}

struct alias* find_alias(char* input)
{
	if (get_first(aliases)->value == NULL)
		return NULL; // Make sure there's aliases to begin with.
	
	struct element* elementptr = get_first(aliases);
	struct alias* aliasptr = elementptr->value;
	struct string* stringptr = aliasptr->alias;
	
	while (elementptr != NULL && aliasptr != NULL)
	{
		if (strcmp(stringptr->data, input) == 0) // Find an alias which string matches the input.
			return aliasptr; // And return a pointer to the alias if found.
			
		elementptr = elementptr->next;
		if (elementptr != NULL) // Increment.
		{
			aliasptr = elementptr->value;
			stringptr = aliasptr->alias;
		}
	}
	return NULL; // If none are found, return NULL.
}

// COMMAND HANDLERS:

int help(char* params)
{
	if (params != NULL && params[0] != 0x0) // If there are parameters to deal with...
	{
		struct command* cmdptr = find_cmd(params);
		if (cmdptr != NULL) // Try to find a command matching them.
		{
			writeline("");
			writeline(cmdptr->format);
			writeline("");
			writeline(cmdptr->help);
		}
		else // If not, print an error message.
		{
			string* errmsg = string_new();
			string_set(errmsg, "Cannot find command: ");
			string_add(errmsg, params);
			writeline(errmsg->data);
			string_free(errmsg);
		}
	}
	else
	{
		// Otherwise print the list.
		int i = 0;
		bool linecomplete = false;
		string* stringbuffer = string_newsz(80);
		
		if (stringbuffer == NULL)
			return ERR_OUT_OF_MEMORY;
		
		writeline("Commands (type help COMMAND for more information):"); // Print a simple header.
		writeline("");
		
		while (i <= num_commands) // Iterate through the command list, displaying them in columns.
		{
			if (linecomplete) // Only print the line when the buffer is built up.
			{
				writeline(stringbuffer->data);
				string_set(stringbuffer, commands[i].name);
				linecomplete = false; // Reset the complete flag.
				i++;
			}
			else
			{
				while (strlen(stringbuffer->data) <= 60) // Add commands onto the line until there's no more space.
				{
					while (strlen(stringbuffer->data) % 20 != 0) 
						string_addchar(stringbuffer, ' '); // Space out into 20-character columns.
					
					if (i >= num_commands) // Stop if there are no more to add.
						break;
					string_add(stringbuffer, commands[i++].name);
				}
				linecomplete = true; // And mark the line as complete once there's no more space.
			}
		}
		string_free(stringbuffer);
	}
	new_prompt();
	return SIG_SUCCESS;
}

int history(char* params)
{
	extern struct element* cmdhistory;
	struct element* historyptr = get_first(cmdhistory);
	string* stringbuffer = string_newsz(TERMINAL_INPUT_SIZE);
	string* strptr;
	int i = 1;
	
	writeline(""); // Write each entry of the history list.
	while (historyptr != NULL)
	{	
		strptr = historyptr->value;

		string_addnum(stringbuffer, i, 10);
		string_add(stringbuffer, ". ");
		string_add(stringbuffer, strptr->data);
		
		writeline(stringbuffer->data);
		string_clear(stringbuffer);
		
		historyptr = historyptr->next;
		i++;
	}
	writeline("");
	
	string_free(stringbuffer); // And free stringbuffer.
	return SIG_SUCCESS;
}

int disk(char* params)
{
	// TODO: Do something with params at some point.
	
	string* stringbuffer = string_newsz(80);
	
	string_set(stringbuffer, "Floppy Drive #1 (Master): ");
	string_add(stringbuffer, floppy_getstring(false));
	writeline(stringbuffer->data); // Print master floppy disk information.
	
	string_set(stringbuffer, "Floppy Drive #2  (Slave): ");
	string_add(stringbuffer, floppy_getstring(true));
	writeline(stringbuffer->data); // Print slave floppy disk information.
	
	writeline("");
	
	if (ata_floating(PRIMARY) == false) // If there are drives on the primary bus.
	{
		string_set(stringbuffer, "Primary ATA #1: ");
		string_add(stringbuffer, ata_tostring(PRIMARY, MASTER));
		writeline(stringbuffer->data);
		
		string_set(stringbuffer, "Primary ATA #2: ");
		string_add(stringbuffer, ata_tostring(PRIMARY, SLAVE));
		writeline(stringbuffer->data);
	}
	if (ata_floating(SECONDARY) == false) // If there are drives on the secondary bus.
	{
		string_set(stringbuffer, "Secondary ATA #1: ");
		string_add(stringbuffer, ata_tostring(SECONDARY, MASTER));
		writeline(stringbuffer->data);
		
		string_set(stringbuffer, "Secondary ATA #2: ");
		string_add(stringbuffer, ata_tostring(SECONDARY, SLAVE));
		writeline(stringbuffer->data);
	}
	
	return SIG_SUCCESS;
}

int list(char* params)
{
	return SIG_SUCCESS;
}

int find(char* params)
{
	return SIG_SUCCESS;
}

int edit(char* params)
{
	return SIG_SUCCESS;
}

int makedir(char* params)
{
	return SIG_SUCCESS;
}

int copy(char* params)
{
	return SIG_SUCCESS;
}

int move(char* params)
{
	return SIG_SUCCESS;
}

int delete(char* params)
{
	return SIG_SUCCESS;
}

int view(char* params)
{
	return SIG_SUCCESS;
}

int memview(char* params)
{
	return SIG_SUCCESS;
}

int cprompt(char* params)
{
	extern string* prompt_string;
	extern size_t prompt_string_length;
	
	// Check to make sure there's something to change.
	if (params != NULL && params[0] != 0x0)
	{
		// Assign the string and update the length.
		string_set(prompt_string, params);
		prompt_string_length = strlen(prompt_string->data);
		return SIG_SUCCESS;
	}
	else
	{
		writeline("Invalid prompt.");
		return SIG_FAIL;
	}
}

int echo(char* params)
{
	writeline(params);
	return SIG_SUCCESS;
}

int cls(char* params)
{
	asm volatile("xchgw %bx, %bx");
	extern int prompt_y;

	kclear(); // Clear the screen.
	kccolor(get_position(0, 0), COLUMNS * ROWS, terminal_color); // Reset the color too.
	prompt_y = -1; // Reset the prompt to -1, so the next newline will put the prompt on row 0.
	
	return SIG_SUCCESS; // And report success.
}

int tcolor(char* params)
{
	int i = 0;
	string* foreground = string_new();
	string* background = string_new();
	
	while (params[i] != ' ' && params[i] != 0x0) // Copy the first word to the foreground string.
	{
		string_addchar(foreground, params[i]);
		i++;
	}
	
	// If we've run into a null terminator before the second word.
	if (params[i] == 0x0)
		goto error;
	
	i++; // Advance past the space.
		
	while (params[i] != ' ' && params[i] != 0x0)
	{
		string_addchar(background, params[i]);
		i++;
	}
	
	// Now that we have both words separated, try to find a matching color for each:
	enum color fg = tocolor(foreground);
	enum color bg = tocolor(background);
	
	if (fg == UNKNOWN || bg == UNKNOWN)
		goto error; // If either parsing fails, jump ahead to error handling.
	
	terminal_color = get_color(fg, bg); // Get the color using the parsed strings.
	cursor_color = get_color(fg, CURSOR_BG);
	
	kccolor(
		get_position(0, 0),
		COLUMNS * ROWS,
		terminal_color);
	
	string_free(foreground); string_free(background);
	return SIG_SUCCESS;
	
	error: // Deal with any errors.
		// Free the strings, don't want a memory leak.
		string_free(foreground); string_free(background);
		writeline("Cannot parse color parameters, see help for format.");
		return SIG_FAIL;
}

int alias(char* params)
{
	if (params[0] == 0x0 || params[0] == ' ') // If there's no parameters, just print the current alias list.
	{
		struct element* elementptr = get_first(aliases);
		struct alias* aliasptr = elementptr->value; // Set the alias pointer to the first entry.
		string* stringbuffer = string_newsz(TERMINAL_INPUT_SIZE); // Allocate a buffer to print aliases in.
		string* strptr; // Get a pointer for individual strings in an alias structure.
		
		while (elementptr != NULL && aliasptr != NULL)
		{	
			// Assemble the message.
			strptr = aliasptr->alias;
			string_add(stringbuffer, strptr->data); 
			string_add(stringbuffer, " is aliased to ");
			strptr = aliasptr->command;
			string_add(stringbuffer, strptr->data);
			writeline(stringbuffer->data); // Print the actual message.
			
			// Increment the pointers.
			elementptr = elementptr->next; 
			if (elementptr != NULL)
				aliasptr = elementptr->value; 
			
			string_clear(stringbuffer); // And clear the stringbuffer.
		}
		string_free(stringbuffer);
		return SIG_SUCCESS;
	}
	else // Otherwise, time to parse.
	{
		string* newalias = string_new();
		string* newcmd = string_new();
		struct element* listptr = get_first(aliases);
		struct alias* aliasptr;
		struct alias* oldptr;
		string* oldstr;
		int i = 0;
		
		while (params[i] != ' ' && params[i] != 0x0) // Parse the first word to be the alias name.
		{
			string_addchar(newalias, params[i]);
			i++;
		}
			
		if (params[i] == 0x0)
			goto error; // If the first word ends on a null terminator, there's no command to alias.
		
		i++; // Advance past the space.
		
		while(params[i] != 0x0) // And parse the rest of the string as a command.
		{
			string_addchar(newcmd, params[i]);
			i++;
		}
		
		while (listptr != NULL) // Find and overwrite any alias name matches, if there are any.
		{
			oldptr = listptr->value;
			oldstr = oldptr->alias;
			
			if (strcmp(newalias->data, oldstr->data) == 0) // If the two aliases' strings are equal, they're duplicates.
			{
				oldstr = oldptr->command; // Record a pointer to the old command.
				oldptr->command = newcmd; // Overwrite the old command with the new one.
				string_free(oldstr); // Free the old, overwritten command string.
				string_free(newalias); // And free the new unused alias string.
				return SIG_SUCCESS; // And return.
			}
			listptr = listptr->next;
		}
		
		aliasptr = (struct alias*)kmalloc(sizeof(struct alias)); // Allocate.
		aliasptr->alias = newalias; // Assign.
		aliasptr->command = newcmd;
		
		if (get_first(aliases)->value == NULL) // Add if there's not yet any values.
			get_first(aliases)->value = aliasptr;
		else
			list_add(aliases, aliasptr); // Add if there's already entries.
		
		return SIG_SUCCESS;
		
		error: // Error handling.
			string_free(newalias); string_free(newcmd);
			writeline("Cannot create alias.");
			return SIG_FAIL; // Free the strings and return with a fail.
	}
}

int cpuinfo(char* params)
{
	unsigned int eax = 0;
	unsigned int ebx = 0;
	unsigned int ecx = 0;
	unsigned int edx = 0;
	string* stringbuffer = string_newsz(127);
	
	if (stringbuffer == NULL)
		return ERR_OUT_OF_MEMORY;
	
	asm volatile("cpuid" // Get the CPUID with eax = 0, or the vendor string.
				:"=b"(ebx), "=c"(ecx), "=d"(edx)
				:"a"(eax));
				
	string_set(stringbuffer, "CPU Vendor: "); // Get the CPU vendor string out of ebx, ecx, and edx.
	string_addchar(stringbuffer, ((ebx >> (8 * 0)) & 0xff)); // Really ugly, but basically all just bitshifts.
	string_addchar(stringbuffer, ((ebx >> (8 * 1)) & 0xff));
	string_addchar(stringbuffer, ((ebx >> (8 * 2)) & 0xff));
	string_addchar(stringbuffer, ((ebx >> (8 * 3)) & 0xff));
	string_addchar(stringbuffer, ((edx >> (8 * 0)) & 0xff));
	string_addchar(stringbuffer, ((edx >> (8 * 1)) & 0xff));
	string_addchar(stringbuffer, ((edx >> (8 * 2)) & 0xff));
	string_addchar(stringbuffer, ((edx >> (8 * 3)) & 0xff));
	string_addchar(stringbuffer, ((ecx >> (8 * 0)) & 0xff));
	string_addchar(stringbuffer, ((ecx >> (8 * 1)) & 0xff));
	string_addchar(stringbuffer, ((ecx >> (8 * 2)) & 0xff));
	string_addchar(stringbuffer, ((ecx >> (8 * 3)) & 0xff));
	
	writeline(stringbuffer->data);
	writeline("");
	
	// TODO: Find out CPU speed?
	
	eax = 1;
	ebx = 0;
	ecx = 0;
	edx = 0;
	
	asm volatile("cpuid" // Next, get the CPUID with eax = 1, the features flags.
				:"=b"(ebx), "=c"(ecx), "=d"(edx)
				:"a"(eax));
				
	writeline("Detected Features:");	
	string_clear(stringbuffer); // Detect and print CPU features.
	if (check_bit(edx, 0))
		string_add(stringbuffer, "fpu ");
	if (check_bit(edx, 1))
		string_add(stringbuffer, "vme ");
	if (check_bit(edx, 3))
		string_add(stringbuffer, "pse ");
	if (check_bit(edx, 4))
		string_add(stringbuffer, "tsc ");
	if (check_bit(edx, 6))
		string_add(stringbuffer, "pae ");
	if (check_bit(edx, 9))
		string_add(stringbuffer, "apic ");
	if (check_bit(edx, 28))
		string_add(stringbuffer, "htt ");
	if (check_bit(edx, 30))
		string_add(stringbuffer, "ia64 ");
	writeline(stringbuffer->data);
	writeline("");
	
	writeline("Instruction Sets:");
	string_clear(stringbuffer); // Detect and print CPU instruction sets.
	if (check_bit(edx, 23))
		string_add(stringbuffer, "mmx ");
	if (check_bit(edx, 25))
		string_add(stringbuffer, "sse ");
	if (check_bit(edx, 26))
		string_add(stringbuffer, "sse2 ");
	if (check_bit(ecx, 0))
		string_add(stringbuffer, "sse3 ");
	if (check_bit(ecx, 9))
		string_add(stringbuffer, "ssse3 ");
	if (check_bit(ecx, 19))
		string_add(stringbuffer, "sse4.1 ");
	if (check_bit(ecx, 20))
		string_add(stringbuffer, "sse4.2 ");
	writeline(stringbuffer->data);
	string_free(stringbuffer);
	return SIG_SUCCESS;
}

int syslog(char* params)
{
	writeline("");
	struct element* syslogptr = getlog();
	string* stringbuffer = string_newsz(80);
	
	syslogptr = get_first(syslogptr);
	while (syslogptr != NULL)
	{
		string_set(stringbuffer, syslogptr->value);
		writeline(stringbuffer->data);
		syslogptr = syslogptr->next; // Write each entry in the system log.
	}
	writeline("");
	return SIG_SUCCESS;
}

int memory(char* params)
{
	string* buffer = string_newsz(63);
	string_add(buffer, "Used Memory: ");
	string_addnum(buffer, used_memory(), 10);
	string_add(buffer, " bytes, in ");
	string_addnum(buffer, used_blocks(), 10);
	string_add(buffer, " blocks.");
	writeline(buffer->data); // Write data on used memory/blocks.
	
	string_set(buffer, "Free Memory: ");
	string_addnum(buffer, free_memory(), 10);
	string_add(buffer, " bytes, in ");
	string_addnum(buffer, free_blocks(), 10);
	string_add(buffer, " blocks.");
	writeline(buffer->data); // Write data on free memory/blocks.
	
	string_set(buffer, "Total Memory: ");
	string_addnum(buffer, free_memory() + used_memory(), 10);
	string_add(buffer, " bytes, in ");
	string_addnum(buffer, total_blocks(), 10);
	string_add(buffer, " blocks.");
	writeline(buffer->data); // Write data on total memory/blocks.
	
	string_free(buffer); // And free the string buffer.
	
	return SIG_SUCCESS;
}

int reboot(char* params)
{
	char x = 0x02;
	disable_interrupts();
	
	while (x & 0x02) // Clear keyboard buffers.
		x = inb(KBD_CMD_PORT);
	outb(KBD_CMD_PORT, 0xFE); // Trigger the reset pin.
	
	return ERR_HARDWARE; // This shouldn't happen.
}

int shutdown(char* params)
{
	extern bool endterm;
	endterm = true;
	return SIG_SUCCESS;
}

int time(char* params)
{
	asm volatile("xchgw %bx, %bx");
	string* stringbuffer = string_newsz(25);
	bool bcd;
	int seconds, minutes, hours;
	
	if (stringbuffer == NULL)
		return ERR_OUT_OF_MEMORY;
	
	// Calculate the uptime.
	seconds = get_ticks() / 1000; // Get the total running seconds from the timer system.
	minutes = seconds / 60; // Calculate the total minutes.
	seconds = seconds % 60; // And recalculate seconds as a remainder of minutes.
	hours = minutes / 60; // Calculate the total hours, like minutes.
	minutes = minutes % 60; // And recalculate hours, just like seconds.
	
	// Print the uptime.
	string_set(stringbuffer, "Uptime: ");
	string_addnum(stringbuffer, hours, 10);
	string_addchar(stringbuffer, ':');
	string_addnum(stringbuffer, minutes, 10);
	string_addchar(stringbuffer, ':');
	string_addnum(stringbuffer, seconds, 10);
	writeline(stringbuffer->data); 
	
	// Check if the RTC is in binary coded decimal mode by seeing if it's the 2000's.
	if (CMOS_read(CMOS_CENTURY, true) == CENTURY) 
		bcd = true; // If so, be sure to convert the rest of the entries.
	else
		bcd = false; // If not, it's probably in flat binary, no need to convert.
	
	// Assemble from CMOS RTC.
	string_set(stringbuffer, "Date/Time: ");
	string_addnum(stringbuffer, CMOS_read(CMOS_CENTURY, bcd), 10);
	string_addnum(stringbuffer, CMOS_read(CMOS_YEARS, bcd), 10);
	string_addchar(stringbuffer, '/');
	string_addnum(stringbuffer, CMOS_read(CMOS_MONTHS, bcd), 10);
	string_addchar(stringbuffer, '/');
	string_addnum(stringbuffer, CMOS_read(CMOS_DAYOFMONTH, bcd), 10);
	string_add(stringbuffer, " - ");
	string_addnum(stringbuffer, CMOS_read(CMOS_HOURS, bcd), 10);
	string_addchar(stringbuffer, ':');
	string_addnum(stringbuffer, CMOS_read(CMOS_MINUTES, bcd), 10);
	string_addchar(stringbuffer, ':');
	string_addnum(stringbuffer, CMOS_read(CMOS_SECONDS, bcd), 10);
	
	if (CMOS_read(CMOS_CENTURY, bcd) != CENTURY) // Hopefully this isn't being run outside of the 2000's.
		writeline("WARNING! Real time may not be accurate."); // Print a warning message.
	writeline(stringbuffer->data); // Print the real time.
	
	string_free(stringbuffer);
	return SIG_SUCCESS; // Free and return.
}

int debug(char* params)
{
	asm volatile ("xchgw %bx, %bx");
	return SIG_SUCCESS;
}

int about(char* params)
{
	writeline("");
	writeline("SSMOS - the Shitty Self Made Operating System");
	writeline("");
	writeline(VERSION);
	writeline(COMPILE_DATE);
	writeline("");
	writeline("Thanks to OSDev.org for the fantastic resources.");
	return SIG_SUCCESS;
}

int version(char* params)
{
	writeline(VERSION);
	writeline(COMPILE_DATE);
	return SIG_SUCCESS;
}