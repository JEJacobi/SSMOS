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
#include "signal.h"
#include "string.h"
#include "timer.h"

bool prompt;
int prompt_x;
int prompt_y;
string* prompt_string;
size_t prompt_string_length;

bool cursor;
int cursor_x;
int cursor_y;
char cursor_char;

char terminal_color;
char cursor_color;
char status_color;

bool endterm;

string* input;
int input_ptr;

struct element* cmdhistory;

static volatile char* cursor_ptr;

void init_terminal()
{
    klog("Setting up terminal..."); // Logging.
    
    cmdhistory = list_new(NULL); // Initialize history list.
    aliases = list_new(NULL); // Initialize alias list.
    
    if (cmdhistory == NULL || aliases == NULL)
        haltdump("FATAL ERROR! Cannot allocate required space for terminal.", ERR_OUT_OF_MEMORY);
    
    input = string_newsz(TERMINAL_INPUT_SIZE); // Allocate a string to act as a command buffer.
    input_ptr = 0;

    terminal_color = get_color(TERMINAL_FG, TERMINAL_BG); // Set the terminal's default color.
    
    prompt = true; // Setup the prompt.
    prompt_string = string_new();
    string_set(prompt_string, PROMPT_STRING);
    prompt_string_length = strlen(prompt_string->data);
    
    prompt_x = PROMPT_START_X;
    prompt_y = PROMPT_START_Y;
    
    cursor = true; // Setup the cursor.
    cursor_x = prompt_string_length;
    cursor_y = prompt_y;
    cursor_char = CURSOR_CHAR;
    cursor_color = get_color(CURSOR_FG, CURSOR_BG);
    
    // And lastly, get the color for the terminal status bar.
    status_color = get_color(STATUS_FG, STATUS_BG);
    
    klog("Terminal initialization complete, ready to run."); // More logging.
}

void run_terminal()
{
    endterm = false;
    
    while(!endterm)
    {   
        draw_prompt();
        kflip(); // Flip the data onto video memory.
        draw_cursor(); // Followed by the cursor.
        handle_input(); // Handle any input, backspaces, executing, parsing, etc.
        ksync(); // And wait for vsync.
    }
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
        if ((unsigned int)cursor_x < prompt_string_length)
            cursor_x = prompt_string_length; // Make sure the cursor never goes behind the prompt.
        
        if(input_ptr > 0) // Make sure input_ptr is bigger than zero.
        {
            input_ptr--; // Move the pointer back to the last one.
            input->data[input_ptr] = 0x0; // And clear the character input_ptr is currently pointing at.
            kputchar(
                get_position(prompt_x + prompt_string_length + input_ptr, prompt_y),
                ' ',
                terminal_color);
        }
    }
    else if (input_ptr < TERMINAL_INPUT_SIZE && t != 0x0) // Otherwise handle text being added (assuming it's less than max input).
    {
        cursor_x++;
        string_addchar(input, t);
        input_ptr++;
    }
}

void parse_input()
{
    struct command* cmdptr;
    int return_status;
    int s = 0;

    if (input_ptr <= 0) // If there's nothing in the buffer, just advance to the next line.
    {
        new_prompt();
        return;
    }
    
    string* cmd_string = string_new(); // String to store the command in.
    string* params = string_new(); // String to store the rest of the input buffer (parameters) in. 
    string* historystr = string_new(); // String to store the executed text and link to the history list.
    
    if (cmd_string == NULL || params == NULL || historystr == NULL)
        haltdump("MEMORY ERROR! Cannot allocate space for terminal", ERR_OUT_OF_MEMORY);
    
    //
    // Handle command history:
    //
    
    string_add(historystr, input->data); // Record the executed command.
    
    if (cmdhistory->value == NULL) // If there's nothing in the list yet, set the first value.
        cmdhistory->value = historystr; 
    else // Otherwise, just append it.
        list_add(cmdhistory, historystr);
    
    //
    // Separate the input buffer to the first space.
    //
    
    while(input->data[s] != ' ' && input->data[s] != 0x0) // Go till a space or null-terminator is encountered.
    {
        string_addchar(cmd_string, input->data[s]); // Copy the string to the array.
        s++;
    }
    
    if (input->data[s] == 0x0)
        goto parse; // If there are no parameters, just jump forward to the parsing.
        
    // Otherwise copy them to a separate array.
    
    s++; // First, advance s to the space after the first space.
    
    while(input->data[s] != 0x0) // Copy till null-terminator.
    {
        string_addchar(params, input->data[s]);
        s++;
    }
    
    //
    // Find a command with the same string.
    //
    
    parse: // Yeah yeah, dirty GOTOs.
    cmdptr = find_cmd(cmd_string->data); // Find a command with the command string gotten earlier.
    
    //
    // If that fails, try looking for an alias.
    //
    
    if (cmdptr == NULL)
    {
        struct alias* aliasptr = find_alias(cmd_string->data);
        if (aliasptr != NULL) // If we've actually found an alias.
        {
            struct string* aliascmd = aliasptr->command; // Yeah this is really ugly, TODO: Refactor eventually.
            string_clear(cmd_string); string_clear(params); // Clear the original command string and parameters.
            int i = 0;
            
            while (aliascmd->data[i] != ' ' && aliascmd->data[i] != 0x0) // Parse until null-terminator or space.
            {
                string_addchar(cmd_string, aliascmd->data[i]); // Copy characters over to the new command string.
                i++;
            }
            
            if (aliascmd->data[i] != 0x0) // If there's more to parse, besides a single command.
            {
                i++;
                while (aliascmd->data[i] != 0x0) // Copy over the rest of the command until a null terminator is reached.
                {
                    string_addchar(params, aliascmd->data[i]);
                    i++;
                }
            }
            
            // Now that the strings are sorted out, try to find a command again.
            cmdptr = find_cmd(cmd_string->data);
        }
    }
    
    //
    // Finally, run a command, executable file, or print an error message.
    //
    
    if(cmdptr != NULL) // Check for a null return, if so, try finding a file?
    {
        return_status = cmdptr->call(params->data);
        // If the command's found, transfer to the handler function.
        
        print_return(return_status, cmdptr->name); // And print the return status if necessary.
    }
    else
    {
        // If not found, try looking for a file?
        
        // And print the return status (when files work).
        
        // If neither works, print an error message and return to the terminal.
        writeline("Cannot find file or command:");
        writeline(cmd_string->data);
    }
    
    string_free(cmd_string); string_free(params); // Free the used strings.
    new_prompt(); // And new prompt.
}

void writeline(char* msg)
{
    int i = 0;
        
    new_prompt();
        
    while (msg[i] != 0x0) // Go until a null terminator is reached.
    {
        kputchar( // Print the characters.
            get_position(i % COLUMNS, prompt_y),
            msg[i],
            terminal_color);
            
        if (i > 0 && i % (COLUMNS - 1) == 0)
            new_prompt(); // Account for any newlines and update the prompt accordingly.
            
        i++;
    }
}

void draw_prompt()
{
    if (prompt)
    {
        kprint( // Print the prompt string first.
            get_position(prompt_x, prompt_y),
            prompt_string->data,
            terminal_color);
        kprint( // The input buffer second.
            get_position(prompt_x + prompt_string_length, prompt_y),    // Print the buffer ahead of the prompt,
            input->data,                                                        // using the precalculated prompt_string_length.
            terminal_color);
    }
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
        kscroll(); // Scroll the screen.
        kccolor(get_position(0, 0), COLUMNS * ROWS, terminal_color); // And be sure to update the colors for the new emptylines.
        prompt_y--;
    }
    
    cursor_y = prompt_y;
    cursor_x = prompt_string_length;
    string_clear(input);
    input_ptr = 0;
}

void print_return(int retstat, char* name)
{
    // If the command exited with something other than SIG_SUCCESS or SIG_FAIL, print the error number.
    if (retstat != SIG_SUCCESS && retstat != SIG_FAIL)
    {
        // Assemble the string.
        string* retmsg = string_new();
        string_set(retmsg, name);
        string_add(retmsg, " returned error code: ");
        string_addnum(retmsg, retstat, 10);
        
        writeline(""); // Print.
        writeline(retmsg->data);
        
        string_free(retmsg); // And free.
    }
}