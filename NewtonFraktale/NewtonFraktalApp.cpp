#include "NewtonFraktalApp.h"
#include "PolyCL.h"

NewtonFraktalApp::NewtonFraktalApp(PolycodeView *view) {
	core = new POLYCODE_CORE(view, 640, 480, false, false, 4, 4, 60);
	Services()->getRenderer()->setClearColor(0, 0, 0, 1);
	CoreServices::getInstance()->getResourceManager()->addArchive("default.pak");
	CoreServices::getInstance()->getResourceManager()->addDirResource("default", false);

	FILE* f;
	if (fopen_s(&f, "newton.cl", "r")!=0){
		return;
	}
	char* buf = (char*)malloc(100 * sizeof(char));
	char* temp = buf;
	int recv_size = 0, total_recv = 0;
	int i = 1;
	while ((recv_size = fread_s(temp, sizeof(char)*100, sizeof(char), 100, f)) > 0){
		total_recv += recv_size;
		buf = (char*)realloc(buf, total_recv + 100 * sizeof(char));
		temp = buf + total_recv;
	}
	buf[total_recv] = '\0';

	vector <size_t> size;
	size.push_back(sizeof(cl_int)*2);
	size.push_back(sizeof(cl_int)*2);
	size.push_back(core->getXRes());
	size.push_back(core->getYRes());
	size.push_back(core->getXRes()*core->getYRes());

	vector <void*> param;

	cl_int* res = (cl_int*)malloc(sizeof(cl_int) * 2);
	res[0] = core->getXRes();
	res[1] = core->getYRes();
	param.push_back(res);

	cl_float* param1 = (cl_float*)malloc(sizeof(cl_float) * 2);
	param1[0] = 3.0;
	param1[1] = 2.0;
	param.push_back(param1);

	pCl = new PolyCL<float>(String(buf),"newtonFraktal",param,size);

	if (!pCl->getSuccess()){
		return;
	}

	fraktal = new Image(core->getXRes(),core->getYRes());
	//fraktal->drawLine(0, 12, 24, 12, Color(255,255,255,255));
	int y = core->getYRes();
	for (int r = 0; r < fraktal->getWidth(); r++){
		for (int i = 0; i < fraktal->getHeight(); i++){
			Number alpha = pCl->getResult()[i + (r*y)];
			fraktal->setPixel(r, i, 1, 1, 1, 1 / alpha);
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
		if (i>=MAX_ROUNDS)
			return i;
		z = z - (c*z*z + c*z + c) / ((c*z)*2 + c);
		i++;
	}
	return i;
}