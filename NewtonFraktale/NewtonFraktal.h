/*
The MIT License (MIT)

Copyright (c) 2017 By Joachim Meyer

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

#include "PolyImage.h"
#include "Polynom.h"
#include <vector>
#include <complex>
#include <ctime>

class NewtonFraktal : public Image {
public:
	NewtonFraktal(int xRes, int yRes);
	~NewtonFraktal();

	void draw();

	void setArea(double x, double y);
	void setCenter(double x, double y);
	void setContrast(double cont);
	void setPolynom(Polynom *p);

	int* getResolution();
	double* getArea();
	double* getCenter();
	double getContrast();
	Polynom* getPolynom();
	Polynom* getDerivation();

	void allocGenRes();

	double *getValues();
	int *getIterations();
	int *getTypes();

	clock_t max_begin, max_end, draw_begin, draw_end;

private:
	__declspec(align(MEM_ALIGN)) double area[2];
	__declspec(align(MEM_ALIGN)) double center[2];

	Polynom* polynom;
	Polynom* derivation = nullptr;

	double contrast;

	double *values;
	int *iterations;
	int *types;
};

