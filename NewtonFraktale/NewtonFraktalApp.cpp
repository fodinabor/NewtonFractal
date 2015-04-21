#include "NewtonFraktalApp.h"
#include <ctime>

#define pi 3.14159265359
extern struct cl_complex;

int colors[] = {
	0x0000FF,
	0x00FF00,
	0x00FFFF,
	0xFF00FF,
	0xFFFF00,
	0x88FF00,
	0x88FF88,
	0xFF88FF,
	0x8888FF,
	0xFFC000,
	0x00FFC0,
	0xC0FFC0,
	0xC000FF,
	0xC0FF00,
	0xFF0000,
};

NewtonFraktalApp::NewtonFraktalApp(PolycodeView *view) {
	core = new POLYCODE_CORE(view, 1500, 1000, false, false, 0, 0, 10);
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

	polynom = new Polynom();
	polynom->addCoefficient(complex<cl_double>(1, 0));
	polynom->addCoefficient(complex<cl_double>(0, 0));
	polynom->addCoefficient(complex<cl_double>(0, 0));
	polynom->addCoefficient(complex<cl_double>(0, 0));
	polynom->addCoefficient(complex<cl_double>(1, 0));
	//polynom->addCoefficient(complex<cl_double>(2, 4));
	//polynom->addCoefficient(complex<cl_double>(0, 0));
	//polynom->addCoefficient(complex<cl_double>(4, 7));
	//polynom->addCoefficient(complex<cl_double>(6, -4));
	//polynom->addCoefficient(complex<cl_double>(1, 0));
	//polynom->addCoefficient(complex<cl_double>(-1, 9));
	//polynom->addCoefficient(complex<cl_double>(1, 6));
	//polynom->addCoefficient(complex<cl_double>(-10, 4));

	derivation = new Polynom((*polynom));
	derivation->differentiate();
	polynom->printPolynom();
	derivation->printPolynom();

	//polynom->getValue(complex<cl_double>(10, 10));

	//findZeros(polynom, derivation, complex<cl_double>(10, 10));

	res = new cl_int[2];
	res[0] = core->getXRes()*2;
	res[1] = core->getYRes()*2;

	zoom = new cl_int[2];
	zoom[0] = 50;
	zoom[1] = 50;

	centerCL = new cl_double[2];
	centerCL[0] = 0;
	centerCL[1] = 0;

	fraktal = new Image(res[0], res[1]);

	cl_int paramc[] = { polynom->getNumCoefficients(), derivation->getNumCoefficients() };

	genCL = new NewtonFraktalCLGeneration(zoom, polynom->getCLCoefficients(), derivation->getCLCoefficients(), paramc, res);
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

	Image *centerSelector = new Image(20,20);
	centerSelector->fill(Color(0.0, 0.0, 0.0, 0.0));
	centerSelector->drawLine(0, 10, 20, 10, Color(1.0,0.0,0.0,1.0));
	centerSelector->drawLine(10, 0, 10, 20, Color(1.0, 0.0, 0.0, 1.0));

	centerSel = new SceneImage(centerSelector);
	centerSel->blendingMode = Renderer::BLEND_MODE_NORMAL;
	centerSel->visible = false;
	centerSel->setPosition(core->getXRes() / 2, core->getYRes() / 2);
	ui->addEntity(centerSel);

	core->getInput()->addEventListener(this, InputEvent::EVENT_MOUSEUP);
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
			int type, it;
			double conDiv;
			if (genCL && genCL->err == CL_SUCCESS){
				conDiv = genCL->result[x + y * xRes];
				type = genCL->typeRes[x + y * xRes];
				it = genCL->iterations[x + y * xRes];
			} else {
				type = 3;
				conDiv = runNewton(std::complex<double>((double)((x - (double)(xRes / 2)) / zoom[0]), (double)((y - (double)(yRes / 2)) / zoom[1])), type);
			}

			if (type < 15){
				Color col;
				col.setColorHexRGB(colors[type]);
				col.setColorHSV(col.getHue(), col.getSaturation(), 1-conDiv);
				fraktal->setPixel(x, y, col);
			} else {
				fraktal->setPixel(x, y, conDiv, conDiv, conDiv, 1);
			}
		}
	}
	fraktal->saveImage(String::IntToString(time(NULL)) + " " + String::IntToString(zoom[0])  +".png");
}

void NewtonFraktalApp::findZeros(Polynom *pol, Polynom *der, std::complex<cl_double> z){
	std::complex<cl_double> zo(0,0);
	for (int i = 0; i < 1000; i++){
		zo = z;
		z = z - pol->getValue(z) / der->getValue(z);
		if (fabs(z.real() - zo.real()) <= 0.0001 && fabs(z.imag() - zo.imag()) <= 0.0001){
			return;
		}
	}
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

cl_double mapCL(cl_double x, cl_double in_min, cl_double in_max, cl_double out_min, cl_double out_max){
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void NewtonFraktalApp::handleEvent(Event* e){
	if (e->getDispatcher() == redraw){
		if (zoomField->getText().isInteger()){
			cl_int paramc[] = { polynom->getNumCoefficients(), derivation->getNumCoefficients() };
			cl_double *center = new cl_double[2];
			center[0] = mapCL((cl_double)centerSel->getPosition().x, 0, core->getXRes(), -(res[0]) / 2, (res[0]) / 2) / zoom[0] + this->centerCL[0];
			center[1] = mapCL((cl_double)centerSel->getPosition().y, 0, core->getYRes(), -(res[1]) / 2, (res[1]) / 2) / zoom[1] + this->centerCL[1];
			zoom[0] = zoomField->getText().toInteger();
			zoom[1] = zoomField->getText().toInteger();
			genCL->runNewton(zoom, res, center, genCL->params, genCL->paramsD, paramc);
			this->centerCL = center;
			drawFractal();
			centerSel->visible = false;
			centerSel->setPosition(((core->getXRes() / 2) - center[0]), (core->getYRes() / 2) - center[1]);
			scene->removeEntity(sceneFraktal);
			sceneFraktal = new SceneImage(fraktal);
			scene->addChild(sceneFraktal);
		}
	} else if(e->getDispatcher() == core->getInput() && e->getEventCode() == InputEvent::EVENT_MOUSEUP) {
		InputEvent* ie = (InputEvent*)e;
		if (!redraw->mouseOver && !zoomField->getMouseOver()){
			centerSel->setPosition(ie->getMousePosition().x, ie->getMousePosition().y);
			centerSel->visible = true;
		}
	}
}