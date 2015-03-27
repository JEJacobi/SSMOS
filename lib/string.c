#include "string.h"

#include <stddef.h>
#include <stdbool.h>

#include "system.h"
//For memory allocation.

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