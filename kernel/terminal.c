#include "terminal.h"

#include <stddef.h>
#include <stdbool.h>

#include "commands.h"
#include "debug.h"
#include "graphics.h"
#include "math.h"
#include "memory.h"
#include "keyboard.h"
#include "output.h"
#include "string.h"

struct command commands[] = // List of built in command structs, their ID's, help strings, and function pointers:
	{
		{ "help", "Get a list of commands, or more specific information like you're doing now.", &help },
		{ "history", "Display recent (this session) terminal command history.", &history },
		{ "memview", "Display a screen of memory, starting at the specified location.", &memview },
		{ "cls", "Clear the screen.", &cls },
		{ "shutdown", "Stop all tasks and prepare the OS for shutdown.", &shutdown }
	};

char* input;
int input_ptr;

bool prompt;
int prompt_x;
int prompt_y;
char* prompt_string;
size_t prompt_string_length;

bool cursor;
int cursor_x;
int cursor_y;
char cursor_char;

char terminal_color;
char cursor_color;
char status_color;

bool endterm;

static volatile char* cursor_ptr;

void init_terminal()
{
	input = malloc(TERMINAL_INPUT_SIZE); //Allocate a block to act as a command buffer.
	*input = 0x0;
	input_ptr = 0;

	terminal_color = get_color(TERMINAL_FG, TERMINAL_BG); // Set the terminal's default color.
	
	prompt = true; // Setup the prompt.
	prompt_string = PROMPT_STRING;
	prompt_string_length = strlen(PROMPT_STRING);
	
	prompt_x = PROMPT_START_X;
	prompt_y = PROMPT_START_Y;
	
	cursor = true; // Setup the cursor.
	cursor_x = prompt_string_length;
	cursor_y = prompt_y;
	cursor_char = CURSOR_CHAR;
	cursor_color = get_color(CURSOR_FG, CURSOR_BG);
	
	// And lastly, get the color for the terminal status bar.
	status_color = get_color(STATUS_FG, STATUS_BG);
}

void run_terminal()
{
	endterm = false;
	
	while(!endterm)
	{	
		print( // Print the prompt first.
			get_position(prompt_x, prompt_y),
			prompt_string,
			terminal_color);
		print( // The input buffer second.
			get_position(prompt_x + prompt_string_length, prompt_y), 	// Print the buffer ahead of the prompt,
			input,														// using the precalculated prompt_string_length.
			terminal_color);
		
		flip(); // Flip the data onto video memory.
		draw_cursor(); // Followed by the cursor.
		handle_input(); // Handle any input, backspaces, executing, parsing, etc.
		sync(); // And wait for vsync.
	}
}

void terminal_write(char* msg)
{
	//TODO: Print *msg to the line and advance to next free line.
}

void handle_input()
{
	char t = poll_key(); // Get input, this really shouldn't be via polling.
	
	if (t == '\n') // If t == newline (enter), parse input and clear buffer.
	{
		parse_input();
		return;
	}
	else if (t == '\b') // If the key entered is backspace, handle removing chars. (TODO: Also DEL)
	{
		cursor_x--;
		if (cursor_x < prompt_string_length)
			cursor_x = prompt_string_length; // Make sure the cursor never goes behind the prompt.
		
		if(input_ptr > 0) // Make sure input_ptr is bigger than zero.
		{
			input_ptr--; // Move the pointer back to the last one.
			input[input_ptr] = '\0'; // And clear the character input_ptr is currently pointing at.
			putchar(
				get_position(prompt_x + prompt_string_length + input_ptr, prompt_y),
				' ',
				terminal_color);
		}
	}
	else if (input_ptr < TERMINAL_INPUT_SIZE && t != 0x0) // Otherwise handle text being added (assuming it's less than max input).
	{
		cursor_x++;
		input[input_ptr] = t;
		input_ptr++;
		input[input_ptr] = 0x0;
	}
}

void parse_input()
{
	struct command* cmdptr;

	if (input_ptr <= 0) // If there's nothing in the buffer, just advance to the next line.
	{
		new_prompt();
		return;
	}
	
	char cmd_string[TERMINAL_INPUT_SIZE];	// Char array to store the command in.
	char params[TERMINAL_INPUT_SIZE];		// Char array to store the rest of the input buffer (parameters) in.
	int s = 0;
	int s_params = 0;
	
	//
	// Separate the input buffer to the first space.
	//
	
	while(input[s] != ' ' && input[s] != 0x0) // Go till a space or null-terminator is encountered.
	{
		cmd_string[s] = input[s]; // Copy the string to the array.
		s++;
	}
	
	cmd_string[s] = 0x0; // Null-terminate the command string.
	
	if (input[s] == 0x0)
		goto parse; // If there are no parameters, just jump forward to the parsing.
		
	// Otherwise copy them to a separate array.
	
	s++; // First, advance s to the space after the first space.
	
	while(input[s] != 0x0) // Copy till null-terminator.
	{
		params[s_params] = input[s]; 	// Copy the character at s to s_params 
										// (counter for param characters, instead of input buffer).
		s++; s_params++;				// And then increment both.
	}
	
	//
	// Find a command with the same string.
	//
	
	parse: // Yeah yeah, dirty GOTOs.
	cmdptr = find_cmd(&cmd_string[0]); 	// Find a command with the command string gotten earlier.
	
	if(cmdptr != NULL) // Check for a null return, if so, try finding a file?
	{
		print( //TEMP
			get_position(0, 0),
			cmdptr->name,
			terminal_color);
		print( //ALSO TEMP
			get_position(0, 1),
			cmdptr->help,
			terminal_color);
		print( //EVEN MORE TEMP
			get_position(0, 2),
			&cmd_string[0],
			terminal_color);
		print( //THE TEMPEST
			get_position(0, 3),
			&params[0],
			terminal_color);
			
		// If the command's found, transfer to the handler function.
	}
	else
	{
		print( //temp
			get_position(0,0),
			"NO COMMAND FOUND",
			terminal_color);
		// If not found, try looking for a file?
		
		// If neither works, print an error message and return to the terminal.
	}
	new_prompt();
}

void draw_cursor()
{
	if (cursor)
	{
		cursor_ptr = (volatile char*)(VIDEO_MEM + get_position(cursor_x, cursor_y));
		*cursor_ptr = cursor_char;
		cursor_ptr++;
		*cursor_ptr = cursor_color;
	}
}

void new_prompt()
{
	prompt_y++; //TODO: Replace this with something that detects how far the cursor has gone.
	
	while (prompt_y > ROWS - 1)
	{
		scroll();
		prompt_y--;
	}
	
	cursor_y = prompt_y;
	cursor_x = prompt_string_length;
	input_ptr = 0;
	memset(input, 0x0, TERMINAL_INPUT_SIZE);
}

struct command* find_cmd(char* input)
{
	int i;
	for (i = 0; i < NUM_COMMANDS; i++)
	{
		printnum(
			get_position(0, 10),
			strcmp(commands[i].name, input),
			terminal_color,
			10);
		flip();
	
		if (strcmp(commands[i].name, input) == 0) // Find a string that's equal to input.
			return &commands[i]; // If found, return a pointer to the commmand.
	}
	return NULL; // If none are found, return NULL.
}