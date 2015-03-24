#include "NewtonFraktalApp.h"
#include <ctime>

#define pi 3.14159265359

int colors[] = {
	0x0000FF,
	0x00FF00,
	0xFF0000,
	0x99FF99,	
	0x99FF00,	
	0xFF99FF,
	0x9999FF,
	0xFFC000,
	0x00FFFF,
	0xFF00FF,
	0xFFFF00,
};

NewtonFraktalApp::NewtonFraktalApp(PolycodeView *view) {
	core = new POLYCODE_CORE(view, 1000, 1000, false, false, 0, 0, 10);
	Services()->getRenderer()->setClearColor(0, 0, 0, 1);
	CoreServices::getInstance()->getResourceManager()->addArchive("default.pak");
	CoreServices::getInstance()->getResourceManager()->addDirResource("default", false);
	CoreServices::getInstance()->getResourceManager()->addArchive("UIThemes.pak");
	CoreServices::getInstance()->getConfig()->loadConfig("Polycode", "UIThemes/dark/theme.xml");

	SceneLabel::defaultAnchor = Vector3(-1.0, -1.0, 0.0);
	SceneLabel::defaultPositionAtBaseline = true;
	SceneLabel::defaultSnapToPixels = true;
	SceneLabel::createMipmapsForLabels = false;

	Entity::defaultBlendingMode = Renderer::BLEND_MODE_NONE;
	CoreServices::getInstance()->getRenderer()->setTextureFilteringMode(Renderer::TEX_FILTERING_NEAREST);

	//polynom = new Polynom();
	//polynom->addCoefficient(ComplexNumber(1, 1));
	//polynom->addCoefficient(ComplexNumber(0, 0));
	//polynom->addCoefficient(ComplexNumber(0, 0));
	//polynom->addCoefficient(ComplexNumber(-1, 0));
	////polynom->addCoefficient(ComplexNumber(-3, 0));
	////polynom->addCoefficient(ComplexNumber(7, 0));
	////polynom->addCoefficient(ComplexNumber(-2, 0));
	//
	//derivation = new Polynom((*polynom));
	//derivation->differentiate();
	//polynom->printPolynom();
	//derivation->printPolynom();

	param = (cl_double*)malloc(sizeof(cl_double) * 2);
	param[0] = 1;
	param[1] = 0;

	res = new cl_int[2];
	res[0] = core->getXRes() * 6;
	res[1] = core->getYRes() * 6;

	zoom = new cl_int[2];
	zoom[0] = 50;
	zoom[1] = 50;

	fraktal = new Image(res[0], res[1]);

	genCL = new NewtonFraktalCLGeneration(zoom, param, res);
	drawFractal();

	scene = new Scene(Scene::SCENE_2D);
	scene->getDefaultCamera()->setOrthoSize(res[0], res[1]);

	sceneFraktal = new SceneImage(fraktal);
	scene->addChild(sceneFraktal);

	ui = new Scene(Scene::SCENE_2D_TOPLEFT);
	ui->getDefaultCamera()->setOrthoSize(core->getXRes(), core->getYRes());
	ui->rootEntity.processInputEvents = true;

	redraw = new UIButton("Redraw", 60);
	redraw->setPositionX(50);
	redraw->addEventListener(this, UIEvent::CLICK_EVENT);
	ui->addChild(redraw);

	zoomField = new UITextInput(false, 35, 15);
	zoomField->setNumberOnly(true);
	ui->addChild(zoomField);
}

NewtonFraktalApp::~NewtonFraktalApp() {
    
}

bool NewtonFraktalApp::Update() {
	return core->updateAndRender();
}

void NewtonFraktalApp::drawFractal(){
	const int xRes = res[0];
	const int yRes = res[1];
	for (int y = 0; y < fraktal->getHeight(); y++){
		for (int x = 0; x < fraktal->getWidth(); x++){
			int type, conDiv;
			if (genCL && genCL->err == CL_SUCCESS){
				conDiv = genCL->result[x + y * xRes];
				type = genCL->typeRes[x + y * xRes];
			} else {
				type = 3;
				conDiv = runNewton(std::complex<double>((double)((x - (double)(xRes / 2)) / zoom[0]), (double)((y - (double)(yRes / 2)) / zoom[1])), type);
			}

			//if (type == 0){
			//	fraktal->setPixel(x, y, 1.0-0.1*conDiv, 1.0-0.1*conDiv, 0, 1);
			//} else if (type == 1){
			//	fraktal->setPixel(x, y, 0, 0, 1.0 - 0.1*conDiv, 1);
			//} else if (type == 2){
			//	fraktal->setPixel(x, y, 0, 1.0 - 0.1*conDiv, 0, 1);
			//} else if (type == 3){
			//	fraktal->setPixel(x, y, 1.0 - 0.1*conDiv, 0, 0, 1);

			if (type < 11){
				Color col;
				col.setColorHexRGB(colors[type]);
				col.setColorHSV(col.getHue(), col.getSaturation(), 1 / (conDiv*0.1));
				fraktal->setPixel(x, y, col);
			} else {
				fraktal->setPixel(x, y, 1 / (conDiv*0.1), 1 / (conDiv*0.1), 1 / (conDiv*0.1), 1);
			}
		}
	}
	fraktal->saveImage(String::IntToString(time(NULL)) + " " + String::IntToString(zoom[0])  +".png");
}

int NewtonFraktalApp::runNewton(std::complex<double> z, int& type){
	int i;
	std::complex<double> old, p, d;
	std::complex<double> x1 = std::complex<double>(1, 0);
	std::complex<double> x2 = std::complex<double>(-0.5, sin(2 * pi / 3));
	std::complex<double> x3 = std::complex<double>(-0.5, -sin(2 * pi / 3));

	for (i = 0; i < 1500; i++){
		old = z;
		z = z - ((z*z*z) - 1.0) / ((z*z) * 3.0);

		if (fabs(z.real() - x1.real()) <= 0.01 && fabs(z.imag() - x1.imag()) <= 0.01){
			type = 0;
			break;
		} else if (fabs(z.real() - x2.real()) <= 0.01 && fabs(z.imag() - x2.imag()) <= 0.01){
			type = 1;
			break;
		} else if (fabs(z.real() - x3.real()) <= 0.01 && fabs(z.imag() - x2.imag()) <= 0.01){
			type = 2;
			break;
		}

		if (fabs(z.real()) > 1000){
			type = 4;
			break;
		}
	}

	return i;
}

void NewtonFraktalApp::handleEvent(Event* e){
	if (e->getDispatcher() == redraw){
		if (zoomField->getText().isInteger()){
			zoom[0] = zoomField->getText().toInteger();
			zoom[1] = zoomField->getText().toInteger();
			genCL->runNewton(zoom, param, res);
			drawFractal();
			scene->removeEntity(sceneFraktal);
			sceneFraktal = new SceneImage(fraktal);
			scene->addChild(sceneFraktal);
		}
	}
}