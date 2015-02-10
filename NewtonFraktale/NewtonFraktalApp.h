#pragma once

#include "PolycodeView.h"
#include "Polycode.h"
#include "ComplexNumber.h"
#include "PolyCL.h"

#define MAX_ROUNDS 100000

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

	PolyCL<float>* pCl;

	Image* fraktal;
};

