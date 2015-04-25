#include "output.h"

#include <stddef.h>
#include "string.h"

enum color tocolor(string* str)
{
	// Make sure strings are all lower case.
	tolower(str->data);
	
	// Yeah this is pretty horrible.
	if (strcmp(str->data, "black") == 0)
		return BLACK;
	if (strcmp(str->data, "blue") == 0)
		return BLUE;
	if (strcmp(str->data, "green") == 0)
		return GREEN;
	if (strcmp(str->data, "cyan") == 0)
		return CYAN;
	if (strcmp(str->data, "red") == 0)
		return RED;
	if (strcmp(str->data, "magenta") == 0)
		return MAGENTA;
	if (strcmp(str->data, "brown") == 0)
		return BROWN;
	if (strcmp(str->data, "white") == 0)
		return LIGHT_GREY;
	
	// If none are found, return UNKNOWN.
	return UNKNOWN;
}