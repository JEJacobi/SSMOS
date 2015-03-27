// Built-in kernel CLI.

#ifndef KERNEL_TERMINAL
#define KERNEL_TERMINAL

#include <stddef.h>
#include <stdbool.h>
#include "commands.h"

#define PROMPT_STRING			"#>"
#define PROMPT_START_X			0
#define PROMPT_START_Y			14
#define TERMINAL_INPUT_SIZE		256
#define TERMINAL_FG				LIGHT_GREEN
#define TERMINAL_BG				BLACK

#define CURSOR_CHAR 			219
#define CURSOR_FG				LIGHT_GREEN
#define CURSOR_BG				DARK_GREY

#define STATUS_FG				WHITE
#define STATUS_BG				BLUE

void init_terminal();
void start_terminal();

void writeline(char* msg);
void draw_cursor();
void handle_input();
void parse_input();
void draw_prompt();
void new_prompt();

#endif