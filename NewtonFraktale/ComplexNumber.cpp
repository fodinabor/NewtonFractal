#include "ComplexNumber.h"
#include <math.h>

ComplexNumber::ComplexNumber(double real, double imag){
	re = real;
	im = imag;

	phi = atan(imag / real);
	r = sqrt(pow(real,2) + pow(imag,2));
}

ComplexNumber ComplexNumber::createFromPolar(double _r, double _phi){
	r = _r;
	phi = _phi;

	re = cos(phi);
	im = sin(phi);

	return *this;
}