// User-space wrapper library for system calls.
#ifndef LIB_SYSTEM
#define LIB_SYSTEM

#include <stddef.h> //size_t
#include <stdbool.h> //bool

void *malloc(size_t size);								// Allocate a chunk of memory from the heap. Starts uninitialized.
void *realloc(void *ptr, size_t newsz);					// Try to reallocate a previously malloc'd chunk of memory to newsz.
														// Data is retained, although it may be truncated if the new size is smaller.
void free(void *ptr);									// Free said chunk of memory after use.

void memcpy(void *dest, const void *src, size_t sz);	// Copy a chunk of memory from *src to *dest, ensuring no conflicts.
void memset(void *dest, char c, size_t sz);				// Fill a chunk of memory with the value in c.

void sleep(int x);										// Sleep for x milliseconds.
int system(const char *command);						// Executes null terminating *command in the system terminal.

void clear();											// Clear the screen.
void ccolor(int pos, int n, char color);				// Change the color of n characters starting at pos to color.
void putchar(int pos, char c, char color);				// Put a colored character at pos.
void putstring(int pos, char *text, int c, char color);	// Put c characters of the specified string at pos with the color entered.
void print(int pos, char *text, char color);			// Print *string on the terminal.
void printnum(int pos, int num, char color, int base); 	// Print an integer onto the screen at the specified position.

void scroll();											// Scrolls the screen by one row.
void flip();											// Flips the backbuffer onto video memory.
void sync();											// Waits for vertical synchronization before proceeding.

#endif