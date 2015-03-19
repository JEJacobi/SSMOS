#include "terminal.h"

#include <stddef.h>
#include <stdbool.h>

#include "commands.h"
#include "graphics.h"
#include "math.h"
#include "memory.h"
#include "keyboard.h"
#include "output.h"
#include "string.h"

struct command commands[] =
	{
		{ "help", "Get a list of commands, or more specific information like you're doing now.", &help },
		{ "history", "Display recent (this session) terminal command history.", &history },
		{ "shutdown", "Stop all tasks and prepare the OS for shutdown.", &shutdown },
		{ "cls", "Clear the screen.", &cls }
	};

char* input;
int input_ptr;

char terminal_color;
bool cursor;
int cursor_x;
int cursor_y;
char cursor_char;
char cursor_color;

static volatile char* cursor_ptr;
static bool endterm;

void init_terminal()
{
	input = malloc(TERMINAL_INPUT_SIZE); //Allocate a block to act as a command buffer.
	*input = '\0';
	input_ptr = 0;

	terminal_color = get_color(TERMINAL_FG, TERMINAL_BG); // Set the terminal's default color.
	
	cursor = true; // Setup the cursor.
	cursor_x = CURSOR_START_X;
	cursor_y = CURSOR_START_Y;
	cursor_char = CURSOR_CHAR;
	cursor_color = get_color(CURSOR_FG, CURSOR_BG);
}

void start_terminal()
{
	endterm = false;
	
	while(!endterm)
	{
		draw_cursor();
	
		print(
			get_position(cursor_x + 1, cursor_y),
			input,
			terminal_color);
		
		handle_input();
		
		flip();
		sync();
	}
	return;
}

static void handle_input()
{
	static char t = '\0';
	
	/*struct command c1 = { "Test1", "Test2", NULL };
	struct command c2 = { "LONGLONGLONG", "LONGERLONGERLONGERLONGERLONGER", NULL };
	printnum(
		get_position(0, 10),
		sizeof(c1),
		terminal_color,
		10);
	printnum(
		get_position(0, 11),
		sizeof(c2),
		terminal_color,
		10);
	print(
		get_position(0, 13),
		c1.name,
		terminal_color);
	print(
		get_position(0, 14),
		c2.name,
		terminal_color);*/

	t = poll_key(); // Get input, this really shouldn't be via polling.
	
	if (t == '\n') // If t == newline (enter), parse input and clear buffer.
	{
		parse_input();
		return;
	}
	else if (t == '\b') // If the key entered is backspace, handle removing chars. (TODO: Also DEL)
	{
		if(input_ptr > 0) // Make sure input_ptr is bigger than zero.
		{
			input_ptr--; // Move the pointer back to the last one.
			input[input_ptr] = '\0'; // And clear the character input_ptr is currently pointing at.
			putchar(
				get_position(cursor_x + input_ptr + 1, cursor_y),
				' ',
				terminal_color);
			//TODO: Cursor?
		}
	}
	else if (input_ptr < TERMINAL_INPUT_SIZE && t != '\0') // Otherwise handle text being added (assuming it's less than max input).
	{
		input[input_ptr] = t;
		input_ptr++;
		input[input_ptr] = '\0';
	}
}

static void parse_input()
{
	static struct command* cmdptr;

	if (input_ptr <= 0)						// TODO: Advance the cursor or something.
		return;								// If there's nothing in the input buffer, nothing to parse.
	
	char cmd_string[TERMINAL_INPUT_SIZE];	// Char array to store the command in.
	char params[TERMINAL_INPUT_SIZE];		// Char array to store the rest of the input buffer (parameters) in.
	int s = 0;
	int s_params = 0;
	
	//
	// Separate the input buffer to the first space.
	//
	
	while(input[s] != ' ' || input[s] != 0x0) // Go till a space or null-terminator is encountered.
	{
		cmd_string[s] = input[s]; // Copy the string to the array.
		s++;
	}
	
	s++; // Advance s to the space after the first space.
	cmd_string[s] = '\0'; // And also null-terminate the command string.
	
	while(input[s] != 0x0) // Copy till null-terminator.
	{
		params[s_params] = input[s]; 	// Copy the character at s to s_params 
										// (counter for param characters, instead of input buffer).
		s++; s_params++;				// And then increment both.
	}
	
	//
	// Find a command with the same string.
	//
	
	cmdptr = find_cmd(&cmd_string[0]); 	// Find a command with the command string gotten earlier.
	
	if(cmdptr != NULL) // Check for a null return, if so, try finding a file?
	{
		print( //TEMP
			get_position(0, 0),
			cmdptr->help,
			terminal_color);
		// If the command's found, transfer to the handler function.
	}
	else
	{
		// If not found, try looking for a file?
		
		// If neither works, print an error message and return to the terminal.
	}
	return;
}

static void draw_cursor()
{
	if (cursor)
	{
		cursor_ptr = (volatile char*)(VIDEO_MEM + get_position(cursor_x, cursor_y));
		*cursor_ptr = cursor_char;
		cursor_ptr++;
		*cursor_ptr = cursor_color;
	}
}

static struct command* find_cmd(char* input)
{
	int i;
	for (i = 0; i < NUM_COMMANDS; i++)
	{
		if (strcmp(commands[i].name, input) == 0) // Find a string that's equal to input.
			return &commands[i]; // If found, return a pointer to the commmand.
	}
	return NULL; // If none are found, return NULL.
}