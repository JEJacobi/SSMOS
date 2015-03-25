#include "commands.h"

#include <stdbool.h>
#include <stddef.h>
#include "terminal.h"
#include "graphics.h"

const struct command commands[] = // List of built in command structs, their ID's, help strings, and function pointers:
	{
		{ "help", "Get a list of commands, or more specific information like you're doing now.",
		"help (x) - x: given command", &help },
		
		{ "history", "Display recent (this session) terminal command history.", "history - no parameters", &history },
		
		{ "memview", "Display a screen of memory, starting at the specified location.", 
		"memview (x) - x: starting location", &memview },
		
		{ "cls", "Clear the screen.", "cls - no parameters", &cls },
		
		{ "shutdown", "Stop all tasks and prepare the OS for shutdown.", "shutdown - no parameters", &shutdown }
	};
	
const int num_commands = (sizeof commands / sizeof(struct command));

int help(char* params)
{
	if (params != NULL && params[0] != 0x0)
	{
		struct command* cmdptr = find_cmd(params);
		if (cmdptr != NULL)
		{
			writeline("");
			writeline(cmdptr->format);
			writeline("");
			writeline(cmdptr->help);
		}
		else
		{
			writeline("Cannot find command: "); // TODO: Append params to this, will need strcat working.
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
	return SIG_SUCCESS;
}

int history(char* params)
{
	// TODO: Will need some kind of command storage.
	
	// Maybe linked list of char*'s to store commands in?
	// Static arrays would be very inefficient due to the max command size.
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

int shutdown(char* params)
{
	extern bool endterm;
	endterm = true;
	return SIG_SUCCESS;
}