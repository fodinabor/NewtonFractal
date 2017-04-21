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

#include "NewtonFraktal.h"
#include "NewtonFraktalGeneration.h"
#include "PolyBezierCurve.h"
#include <ctime>

NewtonFraktal::NewtonFraktal(int xRes, int yRes) : Image(xRes, yRes, Image::IMAGE_RGBA) { }


NewtonFraktal::~NewtonFraktal() {
	_aligned_free(values);
	_aligned_free(iterations);
	_aligned_free(types);
	//_aligned_free(imageData);
}

void NewtonFraktal::draw() {
	max_begin = clock();

	double maxIters = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			if (types[x + y * width] >= 0) {
				maxIters = MAX(maxIters, iterations[x + y * width]);
			}
		}
	}

	max_end = clock();

	draw_begin = clock();

	BezierCurve* colorCurve = new BezierCurve();
	colorCurve->cacheHeightValues = true;
	colorCurve->setHeightCacheResolution(8192);

	colorCurve->addControlPoint2dWithHandles(maxIters * -0.01, 1.5, maxIters * 0.0, 1.0, maxIters * 0.01, 0.5 * contrast);
	colorCurve->addControlPoint2dWithHandles(maxIters * 0.375 * contrast, 0.01, maxIters * 1.0, 0.0, maxIters * 1.3, -0.01);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int type;
			double conDiv, it;
			conDiv = values[x + y * width];
			type = types[x + y * width];
			it = iterations[x + y * width];

			if (type >= 0) {
				Color col = NewtonFraktalGeneration::colors[type];
				//col.setColorHexRGB(colors[type]);
				conDiv = it + conDiv;

				if (conDiv > maxIters)
					conDiv = maxIters;
				if (conDiv < 0.3)
					conDiv = 0.3;

				conDiv = colorCurve->getYValueAtX(conDiv / maxIters);
				col.setColorHSV(col.getHue(), col.getSaturation(), conDiv);
				setPixel(x, y, col);
			} else {
				if (type == -1)
					setPixel(x, y, 0, 0, 0, 1);
				else
					setPixel(x, y, 1, 1, 1, 1);
			}
		}
	}

	draw_end = clock();
}

void NewtonFraktal::setArea(double x, double y) {
	area[0] = x;
	area[1] = y;
}

void NewtonFraktal::setCenter(double x, double y) {
	center[0] = x;
	center[1] = y;
}

void NewtonFraktal::setContrast(double cont) {
	contrast = cont;
}

void NewtonFraktal::setPolynom(Polynom * p) {
	polynom = p;
}

int *NewtonFraktal::getResolution() {
	__declspec(align(MEM_ALIGN)) int *res = new int[2]{ width, height };
	return res;
}

double * NewtonFraktal::getArea() {
	return area;
}

double * NewtonFraktal::getCenter() {
	return center;
}

double NewtonFraktal::getContrast() {
	return contrast;
}

Polynom * NewtonFraktal::getPolynom() {
	return polynom;
}

Polynom * NewtonFraktal::getDerivation() {
	if (!derivation)
		derivation = polynom->getDerivation();

	return derivation;
}

void NewtonFraktal::allocGenRes() {
	values		= (double*)_aligned_malloc(width * height * sizeof(double), MEM_ALIGN);
	iterations	= (int*)_aligned_malloc(width * height * sizeof(int), MEM_ALIGN);
	types		= (int*)_aligned_malloc(width * height * sizeof(int), MEM_ALIGN);
}

double * NewtonFraktal::getValues() {
	return values;
}

int * NewtonFraktal::getIterations() {
	return iterations;
}

int * NewtonFraktal::getTypes() {
	return types;
}


