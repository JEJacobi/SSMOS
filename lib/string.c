#include "string.h"

#include <stddef.h>
#include <stdbool.h>

#include "system.h"
//For memory allocation.

size_t strlen(const char *str)
{
	int len = 0;
	while (*str != 0x0)
	{
		len++;
		str++;
	}
	return len;
}

void strcpy(char* dest, const char *src)
{
	
}

char *strcat(char *dest, const char *src)
{
	return NULL;
}

char *strncat(char *dest, const char *src, size_t n)
{
	return NULL;
}

int strcmp(const char *str1, const char *str2)
{
	char* x1 = str1;
	char* x2 = str2;
	
	
	return 0;
}

void strrev(char* begin, char* end) // Also shamelessly modified from a K&R offshoot thing.
{
	char x;
	
	while (end > begin) // Swap characters using x as a buffer.
		x = *end, *end-- = *begin, *begin++ = x;
}

void toupper(char *str)
{
	
}

void tolower(char *str)
{
	
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