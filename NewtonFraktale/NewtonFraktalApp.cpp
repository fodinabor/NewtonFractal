#include "NewtonFraktalApp.h"

NewtonFraktalApp::NewtonFraktalApp(PolycodeView *view) {
	core = new POLYCODE_CORE(view, 640, 480, false, false, 4, 4, 60);
	Services()->getRenderer()->setClearColor(0, 0, 0, 1);
	CoreServices::getInstance()->getResourceManager()->addArchive("default.pak");
	CoreServices::getInstance()->getResourceManager()->addDirResource("default", false);

	cl_double* param1 = (cl_double*)malloc(sizeof(cl_double) * 2);
	param1[0] = 4.0;
	param1[1] = 1.0;

	gen = new NewtonFraktalCLGeneration(param1);
	if (gen->err != CL_SUCCESS){
		return;
	}

	int *res = new int[2];
	res[0] = core->getXRes();
	res[1] = core->getYRes();

	int *result = (int*)malloc(res[0] * res[0] * sizeof(int));

	//newtonFraktal(res, param1, result);

	fraktal = new Image(core->getXRes(),core->getYRes());
	//fraktal->drawLine(0, 12, 24, 12, Color(255,255,255,255));
	int yM = core->getXRes();
	for (int y = 0; y < fraktal->getHeight(); y++){
		for (int x = 0; x < fraktal->getWidth(); x++){
			Number alpha = gen->result[x + (y*yM)];
			fraktal->setPixel(x, y, 1, 1, 1, 1 / (1000 / alpha));
		}
	}

	scene = new Scene(Scene::SCENE_2D);
	scene->getDefaultCamera()->setOrthoSize(core->getXRes(), core->getYRes());

	SceneImage *sceneFraktal = new SceneImage(fraktal);
	scene->addChild(sceneFraktal);

}

NewtonFraktalApp::~NewtonFraktalApp() {
    
}

bool NewtonFraktalApp::Update() {
	return core->updateAndRender();
}

Number NewtonFraktalApp::runNewton(ComplexNumber c, ComplexNumber z){
	double i = 0;
	while (z.r < 125){
		if (i>=1000)
			return i;
		z = z - (c*z*z + c*z + c) / ((c*z)*2 + c);
		i++;
	}
	return i;
}

//#pragma OPENCL EXTENSION cl_khr_fp64 : enable

struct complex {
	double im;
	double re;
	double r;
	double phi;
};

struct complex createComplexFromPolar(double _r, double _phi){
	struct complex t;
	t.r = _r;
	t.phi = _phi;

	t.re = cos(t.phi);
	t.im = sin(t.phi);

	return t;
}

struct complex createComplexFromKarthes(double real, double imag){
	struct complex t;
	t.re = real;
	t.im = imag;

	t.phi = atan(imag / real);
	t.r = sqrt(pow(real, 2) + pow(imag, 2));

	return t;
}

struct complex recreateComplexFromKarthes(struct complex t){
	return t = createComplexFromKarthes(t.re, t.im);
}

struct complex recreateComplexFromPolar(struct complex t){
	return t = createComplexFromPolar(t.r, t.phi);
}

struct complex addComplex(const struct complex z, const struct complex c){
	struct complex t;
	t.re = c.re + z.re;
	t.im = c.im + z.re;
	return recreateComplexFromKarthes(t);
}

struct complex subComplex(const struct complex z, const struct complex c){
	struct complex t;
	t.re = z.re - c.re;
	t.im = z.im - c.im;
	return recreateComplexFromKarthes(t);
}

struct complex addComplexScalar(const struct complex z, const double n){
	struct complex t;
	t.re = z.re + n;
	return recreateComplexFromKarthes(t);
}

struct complex subComplexScalar(const struct complex z, const double n){
	struct complex t;
	t.re = z.re - n;
	return recreateComplexFromKarthes(t);
}

struct complex multComplexScalar(const struct complex z, const double n) {
	struct complex t;
	t.re = z.re * n;
	t.im = z.im * n;
	return recreateComplexFromKarthes(t);
}

struct complex multComplex(const struct complex z, const struct complex c) {
	struct complex t;
	t.re = z.re*c.re - z.im*c.re;
	t.im = z.re*c.im + z.im*c.re;
	return recreateComplexFromKarthes(t);
}

struct complex divComplex(const struct complex z, const struct complex c) {
	return createComplexFromPolar(z.r / c.r, z.phi - c.phi);
}

void newtonFraktal( const int* res, const double* param, int* result){
	for (int x = 0; x < res[0]; x++){
		for (int y = 0; y < res[1]; y++){
			const int xRes = res[0];
			const int yRes = res[1];

			struct complex z = createComplexFromKarthes(x - (xRes / 2), y - (yRes / 2));

			struct complex c = createComplexFromKarthes(param[0], param[1]);

			int i = 0;
			while (z.r < 500){
				if (i >= 1000)
					break;
				z = subComplex(z, divComplex(addComplex(addComplex(multComplex(multComplex(z, z), c), multComplex(z, c)), c), addComplex(multComplexScalar(multComplex(z, c), 2), c)));
				//z-(c*z*z + c*z + c) / ((c*z) * 2 + c);

				i++;
			}
			result[x + res[0] * y] = i;
		}
	}
}