/*
The MIT License (MIT)

Copyright (c) 2016 By Joachim Meyer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
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
	String getString();

	void clear();

	static Polynom* readFromString(String polynom);

	struct cl_complex* getCLCoefficients();
private:
	std::vector<complex<cl_double>> coefficients;
};

