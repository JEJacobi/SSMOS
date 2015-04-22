#ifndef LIB_STRING
#define LIB_STRING
#include <stddef.h>

#define MAX_INT_CHARS			12								// The maximum amount of characters an int can occupy as a string.

//
//	Standard C-string operations, mostly ripped wholesale from the standard library.
//

// All copy operations require *dest to have enough space, otherwise bad things happen (probably memory headers being screwed up).
size_t strlen(char *str);								// Get the length of a null-terminated string.
void strcpy(char* dest, char *src);						// Copy null-terminated string *src to *dest.
char *strcat(char *dest, char *src);					// Append a null terminated string to *dest.
char *strncat(char *dest, char *src, size_t n);			// Same as above, but indicate bytes to append by n.

int strcmp(char *str1, char *str2);						// Compare two strings (<0 less, =0 equal, >0 greater).
void strrev(char* begin, char* end);					// Reverse a string from begin to end.

void toupper(char *str);								// Converts all lower-case letters to upper-case.
void tolower(char *str);								// Converts all upper-case letters to lower-case.
int toint(char *str);									// Convert a string into an integer value.
double todbl(char *str);								// Convert a string into a floating point value.
void tostring(int val, char* dest, int base);			// Convert an integer into a string, and put the result in *dest.
														// Dest MUST be large enough to hold sizeof(val) + 1 characters.
																										
//
//	Dynamic string system.
//

// Standard C-string operations should work, although there will be no overflow protection.

typedef struct string
{
	char* data; 	// Avoid directly accessing, if possible.
	size_t size; 	// The size of the allocated memory, NOT the size currently in use. Use strlen + 1 for that.
} string;

// Creation/deletion:
string* string_new(size_t init);						// Creates a new dynamic string.

void string_free(string* str);							// Frees the string and its buffer.

// String operations:
void string_add(string* str, char* data);				// Appends (and expands if necessary), *data onto the string.

void string_addchar(string* str, char c);				// Appends (and expands if necessary), c onto the string.

void string_addnum(string* str, int num, int base);		// Appends integer 'num' to the string.

void string_set(string* str, char* data);				// Clears and sets *str to *data, expanding if necessary.

void string_trim(string* str);							// Trims the string's size down to only what is needed,
														// removing any unused space.
														
void string_clear(string* str);							// Clears the string's buffer to a null terminated empty string.
														// Retain original buffer size, no reallocing.

void string_resize(string* str, size_t newsz);			// Resizes the string to newsz, will erase any overflowing data.

int string_padding(string* str);						// Return how many bytes are left over in the allocated buffer.

int string_used(string* str);							// Return how many bytes of the allocated space have actually been used.

#endif