#pragma once

#include "PolycodeView.h"
#include "Polycode.h"
#include "ComplexNumber.h"
#include "NewtonFraktalCLGeneration.h"
#include "NewtonFraktalCUDAGeneration.h"

using namespace Polycode;

class NewtonFraktalApp {
public:
	NewtonFraktalApp(PolycodeView *view);
	~NewtonFraktalApp();
    
	int runNewton(ComplexNumber c, ComplexNumber z);

	void drawFractal(int* data);

    bool Update();
    
private:
    Core *core;
	Scene *scene;

	int* getMaxAndMin(int *data);

	NewtonFraktalCLGeneration* genCL;
	//NewtonFraktalCUDAGeneration *genCUDA;

	Image* fraktal;
};