#pragma once

#include "PolycodeView.h"
#include "Polycode.h"
#include "ComplexNumber.h"
#include "NewtonFraktalCLGeneration.h"

using namespace Polycode;

class NewtonFraktalApp {
public:
	NewtonFraktalApp(PolycodeView *view);
	~NewtonFraktalApp();
    
	Number runNewton(ComplexNumber c, ComplexNumber z);

    bool Update();
    
private:
    Core *core;
	Scene *scene;

	NewtonFraktalCLGeneration* gen;

	Image* fraktal;
};

void newtonFraktal(const int* res, const double* param, int* result);