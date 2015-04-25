// Commands, definitions, and handlers for the terminal.

#ifndef KERNEL_COMMANDS
#define KERNEL_COMMANDS

struct command
{
	char* name;				// The name and calling string of the command.
	char* help;				// The help string of the command.
	char* format;			// The format for any parameters.
	int (*call)(char*);		// The function pointer to the command.
	
							/* All commands return an int as an error status, 
							and take whatever else was in the terminal as a char*.
							It's left up to the calling function to sort and handle
							these parameters. */
};

#define SIG_SUCCESS		0
#define SIG_FAIL		1

struct command* find_cmd(char* input);

// COMMAND HANDLERS 

int help(char* params);		// Display help strings and list functions.
int history(char* params);	// Display command history.

int list(char* params);		// List files in a specified directory.
int find(char* params);		// Finds any files with the string indicated on the disk.
int edit(char* params);		// Basic ASCII text editor.
int hexedit(char* params);	// Basic hex editor.

int makedir(char* params);	// Creates directories indicated.
int copy(char* params);		// Copies file(s) to the location indicated.
int move(char* params);		// Moves file(s) to the location indicated.
int delete(char* params);	// Delete a file.

int view(char* params);		// View a file in ASCII.
int hexview(char* params);	// View a file's hexadecimal values.
int memview(char* params);	// View the current memory at a certain location.

int cls(char* params);		// Clear the screen.
int tcolor(char* params);	// Sets the terminal's color and refreshes the screen.
int alias(char* params);	// Alias a string as a command.
int cprompt(char* params);	// Change the command prompt to what's entered.
int syslog(char* params);	// Displays the system log.
int memory(char* params);	// Displays operating system memory information.
int shutdown(char* params);	// Shut down the computer.

int debug(char* params);	// Triggers Bochs' magic breakpoint, totally useless running on an actual system.
int about(char* params);	// About SSMOS.
int version(char* params);	// Get the current version of SSMOS and maybe the release date.

// COMMAND LIST

extern struct command commands[];
extern int num_commands;

#endif