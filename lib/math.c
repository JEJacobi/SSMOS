#include "math.h"

const double PI 		= 3.14159265358979323846;
const double TWOPI		= 6.28318530717958647693;
const double PIOVER2	= 1.57079632679489661923;
const double PIOVER4	= 0.78539816339744830962;
const double TORADIANS	= 0.01745329251994329577;
const double TODEGREES	= 57.2957795130823208768;
const double E			= 2.71828182845904523536;

double pow(double x, double pow)
{
	int i; //prepare the locals
	double val = x;
	
	for (i = 0; i < pow; i++)
	{
		val *= x; //multiply x by itself pow times
	}
	return val;
}

double sqrt(double x)
{
	__asm__ ("fsqrt" : "+t" (x)); //x86 exclusive, probably.
	return x;
}

double fmod(double x, double y)
{
	return 0;
}

double abs(double x)
{
	if (x < 0)
		return -x;
	else
		return x;
}

double ceil(double x)
{
	return 0;
}

double floor(double x)
{
	return 0;
}

double sin(double x)
{
	return 0;
}

double cos(double x)
{
	return 0;
}

double tan(double x)
{
	return 0;
}

double min(double x, double y)
{
	if (x <= y)
		return x;
	else
		return y;
}

double max(double x, double y)
{
	if (x >= y)
		return x;
	else
		return y;
}

double torad(double x)
{
	return x * TORADIANS;
}

double todeg(double x)
{
	return x * TODEGREES;
}

double clamp(double x, double min, double max)
{
	if (x > max)
		return max;
	else if (x < min)
		return min;
	else
		return x;
}

double round(double x)
{
	double remainder = fmod(x ,1.0); //get the decimal
	x = floor(x); //truncate value
	if (remainder >= 0.5)
		return x + 1.0;
	else
		return x;
}

double lerp(double v1, double v2, double amount)
{
	return v1 + (v2 - v1) * amount;
}
