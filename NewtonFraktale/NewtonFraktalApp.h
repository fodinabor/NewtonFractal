#pragma once

#include "PolycodeView.h"
#include "Polycode.h"
#include <PolycodeUI.h>
#include "Polynom.h"
#include "NewtonFraktalCLGeneration.h"
#include <complex>
//#include "NewtonFraktalCUDAGeneration.h"

using namespace Polycode;

cl_double mapCL(cl_double x, cl_double in_min, cl_double in_max, cl_double out_min, cl_double out_max);

class NewtonFraktalApp : public EventHandler {
public:
	NewtonFraktalApp(PolycodeView *view);
	~NewtonFraktalApp();
    
	int runNewton(std::complex<double> z, int& type);
	void findZeros(Polynom *pol, Polynom *der, std::complex<cl_double> z);

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
	SceneImage *centerSel;

	cl_int *zoom;
	//cl_double* param;
	cl_double* centerCL;
	cl_int *res;
};