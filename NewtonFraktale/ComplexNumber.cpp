#include "ComplexNumber.h"
#include <math.h>

ComplexNumber::ComplexNumber(double real, double imag){
	re = real;
	im = imag;

	phi = atan(im / re);
	r = sqrt(re*re + im*im);
}

ComplexNumber ComplexNumber::createFromPolar(double _r, double _phi){
	r = _r;
	phi = _phi;

	re = cos(phi)*r;
	im = sin(phi)*r;

	return *this;
}

ComplexNumber ComplexNumber::createFromKarthes(double real, double imag){
	re = real;
	im = imag;

	phi = atan(imag / real);
	r = sqrt(pow(real, 2) + pow(imag, 2));
	
	return *this;
}