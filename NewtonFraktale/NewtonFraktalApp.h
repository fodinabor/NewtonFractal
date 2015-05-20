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

	int* getMaxAndMin(int *data);
	void redrawIt();

	NewtonFraktalCLGeneration* genCL;
	//NewtonFraktalCUDAGeneration *genCUDA;

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
	UIButton *redrawWinButton;
	UIButton *openOptions;
	SceneImage *centerSel;

	cl_int *zoom;
	//cl_double* param;
	cl_double* centerCL;
	cl_int *res;
};