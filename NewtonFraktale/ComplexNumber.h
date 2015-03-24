#pragma once
#include "CL\opencl.h"
#include "math.h"

//struct complex {
//	cl_double im;
//	cl_double re;
//	cl_double r;
//	cl_double phi;
//};

class ComplexNumber {
public:
	ComplexNumber(){ im = 0; re = 0; r = 0; phi = 0; }
	ComplexNumber(double real, double imag);
	virtual ~ComplexNumber(){}

	//kartesische Darstellung
	//Imaginaerteil
	double im;
	//Realteil
	double re;

	//polar Darstellung
	//Polarwinkel
	double phi;
	//Betrag
	double r;

	inline ComplexNumber operator + (const ComplexNumber& n) const {
		return ComplexNumber(re + n.re, im + n.im);
	}

	inline ComplexNumber& operator += (const ComplexNumber& n) {
		return createFromKarthes(re + n.re, im + n.im);
	}

	inline ComplexNumber operator + (const double& n) const {
		return ComplexNumber(re + n, im);
	}

	inline ComplexNumber& operator += (const double& n) {
		return createFromKarthes(re + n, im);
	}

	inline ComplexNumber operator - (const ComplexNumber& n) const {
		return ComplexNumber(re - n.re, im - n.im);
	}

	inline ComplexNumber& operator -= (const ComplexNumber& n) {
		return createFromKarthes(re - n.re, im - n.im);
	}

	inline ComplexNumber operator - (const double& n) const {
		return ComplexNumber(re - n, im);
	}

	inline ComplexNumber& operator -= (const double& n) {
		return createFromKarthes(re - n, im);
	}

	inline ComplexNumber operator * (const double& n) const {
		return ComplexNumber(re*n, im*n);
	}

	inline ComplexNumber& operator *= (const double& n){
		return createFromKarthes(re*n, im*n);
	}

	inline ComplexNumber operator * (const ComplexNumber& n) const {
		return ComplexNumber(re*n.re - im*n.re, re*n.im + im*n.re);
	}

	inline ComplexNumber& operator *= (const ComplexNumber& n){
		return createFromKarthes(re*n.re - im*n.re, re*n.im + im*n.re);
	}

	inline ComplexNumber operator / (const ComplexNumber& n) const {
		ComplexNumber ret;
		return ret.createFromPolar(r / n.r, phi - n.phi);
	}

	inline ComplexNumber& operator /= (const ComplexNumber& n){
		return createFromPolar(r / n.r, phi - n.phi);
	}

	inline ComplexNumber operator ^ (const double& n) const {
		ComplexNumber ret = ComplexNumber(*this);
		for (int i = 0; i < n; i++){
			ret = ret * ret;
		}
		return ret;
	}

	inline ComplexNumber operator= (const ComplexNumber& n){
		im = n.im;
		re = n.re;
		phi = n.phi;
		r = n.r;
		return *this;
	}

	inline bool operator==(const ComplexNumber& n){
		return (fabs(re - n.re) <= 0.1 && fabs(im - n.im)<=0.1);
	}

	ComplexNumber createFromPolar(const double r, const double phi);
	ComplexNumber createFromKarthes(const double real, const double imag);

	//struct complex createStruct(){
	//	struct complex temp = complex();
	//	temp.im = im;
	//	temp.re = re;
	//	temp.r = r;
	//	temp.phi = phi;
	//	return temp;
	//}

protected:

};
