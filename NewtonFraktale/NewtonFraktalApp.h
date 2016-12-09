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

#include "PolycodeView.h"
#include "Polycode.h"
#include <PolycodeUI.h>
#include "Polynom.h"
#include "NewtonFraktalCLGeneration.h"
#include <complex>
#include <ctime>

using namespace Polycode;
#define RESOLUTION 0.0000000001

cl_double mapCL(cl_double x, cl_double in_min, cl_double in_max, cl_double out_min, cl_double out_max);

template<typename T>
T clamp(const T &val, const T &min, const T &max)
{
	return max(min, min(max, val));
}

class NewtonFraktalApp : public EventHandler {
public:
	NewtonFraktalApp(PolycodeView *view);
	~NewtonFraktalApp();
    
	void runNewton(std::vector<double> &result, std::vector<double> &iterations, std::vector<int> &typeRes);
	void findZeros();

	void drawFractal();

    bool Update();
	void handleEvent(Event *e);
    
private:
    Core *core;
	Scene *scene;
	Scene *ui;
	Scene *selScene;

	int* getMaxAndMin(int *data);
	void redrawIt();

	NewtonFraktalCLGeneration* genCL;

	Polynom* polynom;
	Polynom* derivation;

	std::vector<std::complex<cl_double>> zeros;

	Image* fraktal;
	SceneImage *sceneFraktal;

	UIWindow* win;
	UIButton *redraw;
	UITextInput *zoomField;
	UILabel* polyLabel;
	UITextInput *polynomInput;
	UILabel* center;
	UITextInput *centerX;
	UITextInput *centerY;
	UIButton *redrawWinButton;
	UIButton *openOptions;
	SceneImage *centerSel;
	UILabel* zoomL;

	SceneImage *zoomSel;
	bool dragging;
	Vector2 startPoint;

	bool centerDirty;

	double ratio;
	cl_double *zoom;
	//cl_double* param;
	cl_double* centerCL;
	cl_int *res;

	UITreeContainer* treeCont;
	bool clOptionsSet;
	bool clOptionsJustSet;
	bool useCPU;

	clock_t begin, end;
};