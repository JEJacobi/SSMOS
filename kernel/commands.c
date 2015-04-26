#include "commands.h"

#include <stdbool.h>
#include <stddef.h>

#include "date.h"
#include "debug.h"
#include "memory.h"
#include "terminal.h"
#include "graphics.h"
#include "output.h"
#include "list.h"
#include "string.h"
#include "signal.h"
#include "version.h"

struct command commands[] = // List of built in command structs, their ID's, help strings, and function pointers:
	{
		{ "help", "Get a list of commands, or more specific information like you're doing now.",
		"help (x) - x: given command", &help },
		
		{ "history", "Display recent terminal history.", "history - no parameters", &history },
		
		{ "memview", "Display a screen of memory, starting at the specified location.", 
		"memview (x) - x: starting location", &memview },
		
		{ "cls", "Clears the screen.", "cls - no parameters", &cls },
		
		{ "tcolor", "Changes the text and screen color. Valid colors are: black, blue, cyan, green,  magenta, red, brown, and white.", "tcolor (x) (y) - x: foreground, y: background", &tcolor },
		
		{ "alias", "Aliases parameters as a command, however normal commands take precedence.",
		"alias (x) (y) - x: alias name, y: command", &alias },
		
		{ "cprompt", "Changes the terminal prompt to the string entered.", "cprompt (x) - x: new prompt", &cprompt },
		
		{ "syslog", "Displays the recent system log.", "syslog - no parameters", &syslog },
		
		{ "memory", "Displays operating system memory information.", "memory - no parameters", &memory },
		
		{ "shutdown", "Stop all tasks and prepare the OS for shutdown.", "shutdown - no parameters", &shutdown },
		
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
		// TODO: Either implement some kind of pause when scroll is needed, or make displaying the list more efficient.
		int i;
		
		writeline("Commands (type help COMMAND for more information):"); // Print a simple header.
		writeline("");
		
		for (i = 0; i < num_commands; i++) // Iterate through the command list.
			writeline(commands[i].name);
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

int hexedit(char* params)
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

int hexview(char* params)
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

int cls(char* params)
{
	extern int prompt_y;

	kclear(); // Clear the screen.
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
		struct alias* aliasptr;
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
		
		// TODO: Find if there are any matches in the already existing alias list, and overwrite if there are.
		
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

int syslog(char* params)
{
	writeline("");
	struct element* syslogptr = getlog();
	string* stringbuffer;
	
	syslogptr = get_first(syslogptr);
	while (syslogptr != NULL)
	{
		stringbuffer = syslogptr->value;
		writeline(stringbuffer->data);
		syslogptr = syslogptr->next; // Write each entry in the system log.
	}
	writeline("");
	return SIG_SUCCESS;
}

int memory(char* params)
{
	string* buffer = string_new();
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

int shutdown(char* params)
{
	extern bool endterm;
	endterm = true;
	return SIG_SUCCESS;
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