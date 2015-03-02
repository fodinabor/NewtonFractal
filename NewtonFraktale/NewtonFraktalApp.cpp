#include "NewtonFraktalApp.h"

NewtonFraktalApp::NewtonFraktalApp(PolycodeView *view) {
	core = new POLYCODE_CORE(view, 800, 800, false, false, 4, 4, 1);
	Services()->getRenderer()->setClearColor(0, 0, 0, 1);
	CoreServices::getInstance()->getResourceManager()->addArchive("default.pak");
	CoreServices::getInstance()->getResourceManager()->addDirResource("default", false);

	cl_double* param1 = (cl_double*)malloc(sizeof(cl_double) * 2);
	param1[0] = 4;
	param1[1] = 3;

	int *res = new int[2];
	res[0] = core->getXRes();
	res[1] = core->getYRes();

	fraktal = new Image(res[0], res[1]);

	//genCUDA = new NewtonFraktalCUDAGeneration(ComplexNumber(param1[0], param1[1]));
	if (/*!genCUDA->success*/true){
		genCL = new NewtonFraktalCLGeneration(param1);
		if (genCL->err != CL_SUCCESS){
			return;
		}
		drawFractal(genCL->result);
	} else {
		//drawFractal(genCUDA->result);
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

void NewtonFraktalApp::drawFractal(int* data){
	int* result = genCL->getMinMax(data);
	int diff = result[1] - result[0];

	const int xRes = core->getXRes();
	for (int y = 0; y < fraktal->getHeight(); y++){
		for (int x = 0; x < fraktal->getWidth(); x++){
			Number alpha = data[x + y * xRes];
			if (alpha > result[1]-diff/4){
				fraktal->setPixel(x, y, 1, 1, 1, 1 / ((result[1] - diff / 4) / alpha));
			} else if (alpha > result[1] - diff / 2){
				fraktal->setPixel(x, y, 0, 0, 1 / ((result[1] - diff / 2) / alpha), 1);
			} else if (alpha > result[1] - diff){
				fraktal->setPixel(x, y, 0, 1 / ((result[1] - diff) / alpha), 0, 1);
			} else {
				fraktal->setPixel(x, y, 1 / (result[1] / alpha), 0, 0, 1);
			}
		}
	}
}

int NewtonFraktalApp::runNewton(ComplexNumber c, ComplexNumber z){
	int i = 0;
	while (z.r < 125){
		if (i>=1000)
			return i;
		z = z - (c*z*z + c*z + c) / ((c*z)*2 + c);
		i++;
	}
	return i;
}