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

#include "Polycode.h"
#include "NewtonFraktalGlobals.h"
#include "NewtonFraktal.h"
#include <complex>
#include <vector>

using namespace Polycode;

class Polynom;

class NewtonFraktalGenerator {
public:
	NewtonFraktalGenerator();
	virtual ~NewtonFraktalGenerator() {}

	virtual void runNewton(NewtonFraktal *fraktal);

	/**
	* Typ of the generator.
	* Can be of values declared in NewtonFraktalGeneration (e.g. GENERATION_MODE_CPU).
	*/
	int generatorType;

	static bool compComplex(const std::complex<double> z, const std::complex<double> c, double comp);
};

class NewtonFraktalGeneration {
public:
	NewtonFraktalGeneration();
	~NewtonFraktalGeneration();

	/**
	* Generates a NewtonFraktal. All the data is in the passed over fraktal obj.
	* @param fraktal In/Out: should be filled with information about size, pos and polynom before passing over. Is the drawn Fractal after generation.
	* @param mode In: Indicates how the generation of the Newton Fractal should be run. Possible values: GENERATION_MODE_CPU, GENERATION_MODE_CL
	*/
	void generate(NewtonFraktal *fraktal, int mode);

	void registerGenerator(NewtonFraktalGenerator* newGen, int generatorType);

	static vector<Color> colors;

	
	///////////////
	// Constants //
	///////////////

	static const int GENERATION_MODE_CPU = 0;
	static const int GENERATION_MODE_CL = 1;

	static const vector<int> colorsHex;

private:
	vector<NewtonFraktalGenerator*> generators;
	
	NewtonFraktalGenerator* getGeneratorForMode(int mode);

	//////////////////
	// Misc methods //
	//////////////////

};
