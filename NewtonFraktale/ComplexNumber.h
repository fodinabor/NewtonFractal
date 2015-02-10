#pragma once
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

	inline ComplexNumber operator + (const int& n) const {
		return ComplexNumber(re + n, im);
	}

	inline ComplexNumber operator - (const ComplexNumber& n) const {
		return ComplexNumber(re - n.re, im - n.im);
	}

	inline ComplexNumber operator - (const int& n) const {
		return ComplexNumber(re - n, im);
	}

	inline ComplexNumber operator * (const int& n) const {
		return ComplexNumber(re*n, im*n);
	}

	inline ComplexNumber operator * (const ComplexNumber& n) const {
		return ComplexNumber(re*n.re - im*n.re, re*n.im + im*n.re);
	}

	inline ComplexNumber operator / (const ComplexNumber& n) const {
		ComplexNumber ret;
		return ret.createFromPolar(r / n.r, phi - n.phi);
	}

	ComplexNumber createFromPolar(const double r, const double phi);

protected:

};

