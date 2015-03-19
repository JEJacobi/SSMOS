#ifndef LIB_STRING
#define LIB_STRING
#include <stddef.h>

size_t strlen(const char *str);								// Get the length of a null-terminated string.
void strcpy(char* dest, const char *src);					// Copy null-terminated string *src to *dest.
char *strcat(char *dest, const char *src);					// Append a null terminated string to *dest.
char *strncat(char *dest, const char *src, size_t n);		// Same as above, but indicate bytes to append by n.
int strcmp(const char *str1, const char *str2);				// Compare two strings (<0 less, =0 equal, >0 greater).
void strrev(char* begin, char* end);						// Reverse a string from begin to end.

void toupper(char *str);									// Converts all lower-case letters to upper-case.
void tolower(char *str);									// Converts all upper-case letters to lower-case.
int toint(char *str);										// Convert a string into an integer value.
double todbl(char *str);									// Convert a string into a floating point value.
void tostring(int val, char* dest, int base);				// Convert an integer into a string, and put the result in *dest.
															// Dest MUST be large enough to hold sizeof(val) + 1 characters.
#endif