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

#include "NewtonFraktalView.h"
#include "Polycode.h"
#include <PolycodeUI.h>
#include "Polynom.h"
#include "NewtonFraktalCLGenerator.h"
#include "NewtonFraktalGlobals.h"
#include <complex>
#include <ctime>

using namespace Polycode;

cl_double mapCL(cl_double x, cl_double in_min, cl_double in_max, cl_double out_min, cl_double out_max);

template<typename T>
T clamp(const T &val, const T &min, const T &max)
{
	return max(min, min(max, val));
}

class NewtonFraktalGeneration;

class NewtonFraktalApp : public EventHandler {
public:
	NewtonFraktalApp(NewtonFraktalView *view);
	~NewtonFraktalApp();

    bool Update();
	void handleEvent(Event *e);
    
private:
    Core *core;
	Scene *scene;
	Scene *ui;
	Scene *selScene;

	void redrawIt();
	void generateRandom();

	NewtonFraktalGeneration* gen;

	Polynom* polynom;
	Polynom* derivation;

	std::vector<std::complex<cl_double>> zeros;

	SceneImage *sceneFraktal;

	UIWindow* win;
	UIButton *redraw;
	UITextInput *zoomField;
	UILabel* polyLabel;
	UITextInput *polynomInput;
	UILabel* center;
	UITextInput *centerX;
	UITextInput *centerY;
	UILabel* contrastLabel;
	UIHSlider *contrastSlider;
	UITextInput *contrastInput;
	UILabel* resLabel;
	UILabel* resTimesLabel;
	UITextInput *resXInput;
	UITextInput *resYInput;
	UIButton *redrawWinButton;
	UIButton *randomGenButton;
	UIButton *openOptions;
	SceneImage *centerSel;
	UILabel* zoomL;
	UILabel* nonDoubleLabel;

	SceneImage *zoomSel;
	bool dragging;
	Vector2 startPoint;

	Number contrastValue;

	bool centerDirty;

	double ratio;
	cl_double *zoom;
	//cl_double* param;
	cl_double* centerCL;
	cl_int *res;

	UITreeContainer* treeCont;
	bool clOptionsSet;
	bool clOptionsJustSet;
	int generationMode;

	clock_t begin, end;

	Number timer;

	std::vector<Color> colors;
};