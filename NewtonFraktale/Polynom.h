#pragma once
#include <vector>
#include <CL\opencl.h>
#include <complex>
#include <PolyString.h>

using namespace Polycode;
using namespace std;

struct cl_complex {
	cl_double im;
	cl_double re;
	cl_double r;
	cl_double phi;

	cl_char isPolar;
	cl_char isKarthes;
};

class Polynom {
public:
	Polynom();
	~Polynom();

	void addCoefficient(complex<cl_double> nC);
	void differentiate();

	complex<cl_double> getValue(complex<cl_double> nC);

	int getNumCoefficients();
	complex<cl_double> getCoefficient(int idx);

	String printPolynom();

	static Polynom* readFromString(String polynom);

	struct cl_complex* getCLCoefficients();
private:
	std::vector<complex<cl_double>> coefficients;
};

