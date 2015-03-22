#include "commands.h"
#include "terminal.h"

// COMMANDS:

int help(char* params)
{
	// TODO: Write the contents of commands to the screen if there's no params, 
	// and the detailed help for the command if there is.
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
	return SIG_SUCCESS;
}

int cls(char* params)
{
	//TODO: Clear the screen and reset the prompt/cursor.
	return SIG_SUCCESS;
}

int shutdown(char* params)
{
	extern bool endterm;
	endterm = true;
	return SIG_SUCCESS;
}