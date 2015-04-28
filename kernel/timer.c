#include "timer.h"

#include "hardware.h"
#include "math.h"

unsigned long ticks = 0; // Global counter variable, increases by one every millisecond.

void timer_init()
{
	asm volatile ("cli");
	
	// Send the reinitialize byte to activate Channel 0 as a rate generator.
	outb(PIT_COMMAND, PIT_INIT);
	
	// Set the new frequency divider in lowbyte/highbyte mode.
	outb(PIT_CHANNEL_0, PIT_DIV_LOW);
	outb(PIT_CHANNEL_0, PIT_DIV_HIGH);
}

void timer_IRQ()
{
	ticks++;
}

unsigned long get_ticks()
{
	return ticks;
}

unsigned int get_seconds()
{
	// Get a rounded total of seconds.
	return round(ticks / 1000);
}

void ksleep(int milliseconds)
{
	if (milliseconds <= 0)
		return; // Error check, make sure milliseconds is positive.
	
	// Set ticksgoal, the tick count to do nothing until.
	unsigned long ticksgoal = ticks + milliseconds;
	
	while (ticks < ticksgoal) { } // Wait until ticksgoal is met.
}