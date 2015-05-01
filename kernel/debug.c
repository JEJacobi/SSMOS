#include "debug.h"

#include "graphics.h"
#include "output.h"
#include "list.h"
#include "string.h"

struct element* systemlog = NULL;

void haltdump(char* msg)
{
	//TODO: Print registers, message, and stack to the screen. See how Linux does it, basically.
	*((volatile char*)(VIDEO_MEM)) = 'X'; // TEMP
	while(true) { }
}

void klog(char* msg)
{
	if (systemlog == NULL)
		systemlog = list_new(msg);
	else
		list_add(systemlog, msg);
}

struct element* getlog()
{
	return systemlog;
}