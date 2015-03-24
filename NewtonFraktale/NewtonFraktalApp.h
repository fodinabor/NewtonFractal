#pragma once

#include "PolycodeView.h"
#include "Polycode.h"
#include <PolycodeUI.h>
#include "Polynom.h"
#include "NewtonFraktalCLGeneration.h"
#include <complex>
//#include "NewtonFraktalCUDAGeneration.h"

using namespace Polycode;

class NewtonFraktalApp : public EventHandler {
public:
	NewtonFraktalApp(PolycodeView *view);
	~NewtonFraktalApp();
    
	int runNewton(std::complex<double> z, int& type);

	void drawFractal();

    bool Update();
	void handleEvent(Event *e);
    
private:
    Core *core;
	Scene *scene;
	Scene *ui;

	int* getMaxAndMin(int *data);

	NewtonFraktalCLGeneration* genCL;
	//NewtonFraktalCUDAGeneration *genCUDA;

	Polynom* polynom;
	Polynom* derivation;
	Image* fraktal;
	SceneImage *sceneFraktal;

	UIButton *redraw;
	UITextInput *zoomField;

	cl_int *zoom;
	cl_double* param;
	cl_int *res;
};