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

	//virtual void calcZeros(NewtonFraktal *fraktal);
	virtual void runNewton(NewtonFraktal *fraktal);

	/**
	* Type of the generator.
	* Can be of values declared in NewtonFraktalGeneration (e.g. GENERATION_MODE_CPU).
	*/
	int generatorType;

	int status;

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
	* @param save In Opt: If set to true the fractal will be saved automatically with a timestamp as name
	*/
	void generate(NewtonFraktal *fraktal, int mode, bool save = true);

	/**
	* Generates a NewtonFraktal. All the data is in the passed over fraktal obj.
	* @param fraktal In/Out: should be filled with information about size, pos and polynom before passing over. Is the drawn Fractal after generation.
	* @param generator In: the generation will use the passed over generator.
	* @param save In Opt: If set to true the fractal will be saved automatically with a timestamp as name
	*/
	void generate(NewtonFraktal *fraktal, NewtonFraktalGenerator *generator, bool save = true);

	/**
	* Generates a series of NewtonFraktals, that dives deeper into the Fractal (or other way round).
	* @param area: 2D BezierCurve time, area (size)
	* @param center: 3D BezierCurve time, x, y (of the center)
	* @param framerate: the framerate at which the zoom shall run (in 1sek/framerate)
	* @param duration: how long the zoom shall take (in sec)
	* @param resultion: field with two entries x, y resolution of the fractals
	* @param polynom: the polynom used for generation
	*/
	void generateZoom(BezierCurve *areaCurve, BezierCurve* centerCurve, int framerate, int duration, int* resolution, Polynom* polynom, double contrast);

	NewtonFraktalGenerator* registerGenerator(NewtonFraktalGenerator* newGen, int generatorType);
	void setDefaultGenerationMode(int newDef);
	int getDefaultGenerationMode();

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

	int defaultGenerationMode;
	NewtonFraktalGenerator *defaultGenerator;

	//////////////////
	// Misc methods //
	//////////////////

	static String getExePath();
};
