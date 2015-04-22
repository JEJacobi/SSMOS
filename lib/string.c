#include "string.h"

#include <stddef.h>
#include <stdbool.h>

#include "system.h"
//For memory allocation.

//
//	C-string standard library implementation.
//

size_t strlen(char *str)
{
	int len = 0;
	while (*str != 0x0)
	{
		len++;
		str++;
	}
	return len;
}

void strcpy(char* dest, char *src)
{
	char* x = dest;
	char* y = src;
	
	while (*y != 0x0) // Copy until null-terminator, as usual.
	{
		*x = *y; 	// Copy a character.
		x++; y++;	// And increment to the next one.
	}
	*x = 0x0; // And lastly, close out the destination string with a null-terminator.
}

char *strcat(char *dest, char *src)
{
	char* x = dest;
	char* y = src;
	
	while (*x != 0x0)
		x++; // Set x to the null-terminator of the destination string, as an offset to copy to.
	
	while (*y != 0x0)
	{
		*x = *y; // Copy each character of y to the destination offset.
		x++; y++; // And increment both.
	}
	
	*x = 0x0; // Lastly again, null-terminate *dest.
	return dest;
}

char *strncat(char *dest, char *src, size_t n)
{
	int i;
	char* x = dest;
	char* y = src;
	
	while (*x != 0x0)
		x++; // Offset X, same as strcat.
		
	for (i = 0; i < n; i++)
	{
		if (*y == 0x0) // If there's nothing left to copy, return.
			return dest;
	
		*x = *y; // Copy and increment the pointers.
		x++; y++;
	}
	return dest;
}

int strcmp(char *str1, char *str2)
{
	char* x = str1;
	char* y = str2;
	
	while (*x != 0x0)
	{
		if (*y == 0x0)
			return 1; // If str2 runs out before str1, str1 is greater.
		if (*y > *x)
			return -1; // If str2 is greater than str1, return that.
		if (*x > *y)
			return 1; // If str1 is greater than str2, return that.
			
		x++; y++; // Increment both.
	}
	
	if (*y != 0x0)
		return -1; 	// Once str1 has ended, check str 2 for also ending. 
					// If it hasn't, it's greater, so return -1.
		
	return 0; // If it still hasn't, both are exactly equal, so return 0.
}

void strrev(char* begin, char* end) // Also shamelessly modified from a K&R offshoot thing.
{
	char x;
	
	while (end > begin) // Swap characters using x as a buffer.
		x = *end, *end-- = *begin, *begin++ = x;
}

void toupper(char *str)
{
	while (*str != 0x0) // Go until null-terminator.
	{
		if (*str <= 122 && *str >= 97) // Lowercase ranges from 97-122.
			*str -= 32; // The magic number to convert lower-case ASCII to upper-case ASCII.
			
		str++;
	}
}

void tolower(char *str)
{
	while (*str != 0x0) // Go until null-terminator.
	{
		if (*str <= 90 && *str >= 65) // Uppercase ranges from 65-90.
			*str += 32; // Also the magic number for the other way around (but add it this time).
			
		str++;
	}
}

int toint(char *str)
{
	return 0;
}

double todbl(char *str)
{
	return 0;
}

void tostring(int val, char* dest, int base) // Shamelessly modified from a modification of K&R itoa.
{
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	char* buffer = dest;
	int sign;

	if (base < 2 || base > 35)
		return; // Check for an invalid base and abort if found.
		
	if ((sign = val) < 0)
		val = -val; // Check for sign and make sure val's positive.
	
	do 	// Convert number to string, reversed at first.
	{
		*buffer++ = num[val%base];
	} while (val /= base);
	
	if (sign < 0)
		*buffer++ = '-';
		
	*buffer = '\0';
	
	strrev(dest, buffer - 1);
}

//
//	Dynamic string system:
//

string* string_new(size_t init)
{
	// Initialize the actual structure.
	string* str = (string*)malloc(sizeof(string));
	if (str == NULL)
		return NULL;
		
	if (init < 0)
		init = 0; // Clamp the size to zero, which would just be the null terminator.
	
	// Initialize the values.
	str->size = init + 1; // +1 because strlen doesn't take space for the null-terminator into account.
	str->data = (char*)malloc(str->size); // Allocate the string buffer with the desired initial size.
	str->data[0] = 0x0; // Null terminate the initial emptystring.
	
	return str;
}

void string_free(string* str)
{
	free(str->data);
	free(str);
}

void string_add(string* str, char* data)
{
	// String buffer must have enough space for both char*'s and a null terminator.
	if (str->size < strlen(str->data) + strlen(data) + 1)
		string_resize(str, strlen(str->data) + strlen(data) + 1); // Resize to fit.
	
	strcat(str->data, data); // And just use the standard strcat to append.
}

void string_addchar(string* str, char c)
{
	if (str->size <= strlen(str->data) + 2) // Only adding one character, no need to strlen the message.
		string_resize(str, str->size + 1); // Resize if necessary.
	
	int x = 0;
	while(str->data[x] != 0x0)
		x++; // Set index x to the null-terminator.
	str->data[x] = c; // Append character c.
	x++;
	str->data[x] = 0x0; // And null-terminate as usual.
}

void string_addnum(string* str, int num, int base)
{
	char* numbuffer = (char*)malloc(MAX_INT_CHARS);
	tostring(num, numbuffer, base);
	string_add(str, numbuffer);
	free(numbuffer);
}

void string_set(string* str, char* data)
{
	if(strlen(data) + 1 > str->size) // If the new message is larger than the current capacity...
		string_resize(str, strlen(data) + 1); // Expand the string's buffer to fit if needed.
		
	strcpy(str->data, data); // And copy over the new string.
}

void string_trim(string* str)
{
	string_resize(str, str->size - string_padding(str)); // Resize the string to have no padding space.
}

void string_clear(string* str)
{
	int i;
	
	for (i = 0; i < str->size; i++)
	{
		str->data[i] = 0x0; // Overwrite null-terminators into every space.
	}
}

void string_resize(string* str, size_t newsz)
{
	size_t oldsz = str->size; // Temporarily store the old size.
	
	// Reallocate the block and update the pointer. Realloc also takes care of copying data over.
	str->data = realloc(str->data, newsz);
	
	if (newsz < oldsz) // If the block has been truncated, the null-terminator's been as well.
	{
		str->data[newsz - 1] = 0x0; // Fix this.
		// TODO: Check if -1 needs to be here.
	}
}

int string_padding(string* str)
{
	return str->size - (strlen(str->data) + 1); // Return however many bytes are leftover in the allocated space.
}

int string_used(string* str)
{
	return str->size - string_padding(str);
}