#include "commands.h"

#include <stdbool.h>
#include <stddef.h>
#include "date.h"
#include "memory.h"
#include "terminal.h"
#include "graphics.h"
#include "output.h"
#include "string.h"
#include "version.h"

struct command commands[] = // List of built in command structs, their ID's, help strings, and function pointers:
	{
		{ "help", "Get a list of commands, or more specific information like you're doing now.",
		"help (x) - x: given command", &help },
		
		{ "history", "Display recent terminal command history.", "history - no parameters", &history },
		
		{ "memview", "Display a screen of memory, starting at the specified location.", 
		"memview (x) - x: starting location", &memview },
		
		{ "cls", "Clears the screen.", "cls - no parameters", &cls },
		
		{ "tcolor", "Changes the text and screen color. Valid colors are: Black, Blue, Cyan, Green,  Magenta, Brown, and White.", "tcolor (x) (y) - x: foreground, y: background", &tcolor },
		
		{ "alias", "Aliases parameters as a command, however normal commands take precedence.",
		"alias (x) (y) - x: alias name, y: command", &alias },
		
		{ "cprompt", "Changes the terminal prompt the string entered.", "cprompt (x) - x: new prompt", &cprompt },
		
		{ "shutdown", "Stop all tasks and prepare the OS for shutdown.", "shutdown - no parameters", &shutdown },
		
		{ "debug", "Used to trigger an external emulator debugger.", "debug - no parameters", &debug },
		
		{ "about", "The SSMOS about page.", "about - no parameters", &about },
		
		{ "version", "Displays the current version of SSMOS.", "version - no parameters", &version }
	};
	
int num_commands = (sizeof commands / sizeof(struct command));

struct command* find_cmd(char* input)
{
	int i;
	for (i = 0; i < num_commands; i++)
	{
		if (strcmp(commands[i].name, input) == 0) // Find a string that's equal to input.
			return &commands[i]; // If found, return a pointer to the commmand.
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
			char* errmsg = "Cannot find command: ";
			char* error = malloc(sizeof(errmsg) + TERMINAL_INPUT_SIZE);
			strcpy(error, errmsg);
			strcat(error, params);
			writeline(error); // Fancy way of printing a message and what was entered.
			free(error);
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
	// TODO: Will need some kind of command storage.
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
	extern char* prompt_string;
	extern size_t prompt_string_length;
	if (params != NULL && params[0] != 0x0)
	{
		// TODO: Dynamically allocate the prompt space, and make sure this doesn't cause an overflow.
	}
	else
	{
		writeline("Invalid prompt.");
	}
	return SIG_SUCCESS;
}

int cls(char* params)
{
	extern int prompt_x;
	extern int prompt_y;

	clear(); // Clear the screen.
	prompt_y = -1; // Reset the prompt to -1, so the next newline will put the prompt on row 0.
	
	return SIG_SUCCESS; // And report success.
}

int tcolor(char* params)
{
	return SIG_SUCCESS;
}

int alias(char* params)
{
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
	writeline("SSMOS - the S**** Self Made Operating System");
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
	return SIG_SUCCESS;
}