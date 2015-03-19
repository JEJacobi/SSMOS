//MATH LIBRARY
#ifndef LIB_MATH
#define LIB_MATH

extern const double PI;
extern const double TWOPI;
extern const double PIOVER2;
extern const double PIOVER4;
extern const double TORADIANS;
extern const double TODEGREES;
extern const double E;

double pow(double x, double pow);			//Returns X^Y
double sqrt(double x);						//Returns sqrt(x)
double fmod(double x, double y);			//Returns the floating % of X

double abs(double x);				//Returns the absolute value of x
double ceil(double x);				//Returns the smallest following integer
double floor(double x);				//Returns the largest previous integer

double sin(double x);						//Returns the sine of x in radians
double cos(double x);						//Returns the cosine of x in radians
double tan(double x);						//Returns the tangent of x in radians

double min(double x, double y);		//Returns the smaller of two numbers
double max(double x, double y);		//Returns the larger of two numbers

double torad(double x);						//Converts x in degrees to radians
double todeg(double x);						//Converts x in radians to degrees

double clamp(double x,
		double min, double max);			//Clamps x between min and max
		
double round(double x);						//Returns x rounded to the nearest integer

double lerp(double v1,						//Linearly interpolates between v1 and v2 by amount.
			double v2,
			double amount);

#endif