#include "NewtonFraktalApp.h"
#include <ctime>
#include <time.h>
#include <cmath>

#define pi 3.14159265359
extern struct cl_complex;
extern bool compComplex(const std::complex<cl_double> z, const std::complex<cl_double> c, double comp);

int colors[] = {
	0x0000FF,
	0x00FF00,
	0xFF0000,
	0xFFFF00,
	0x00FFFF,
	0xFF00FF,
	0x88FF00,
	0x88FF88,
	0xFF88FF,
	0x8888FF,
	0xFFC000,
	0x00FFC0,
	0xC0FFC0,
	0xC000FF,
	0xC0FF00,
	0x9900ff,
	0x0099ff,
	0xff0099,
	0xff9900
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

	Services()->getLogger()->setLogToFile(true);

	clOptionsSet = false;

	polynom = new Polynom();
	polynom->addCoefficient(complex<cl_double>(-1, 0));
	polynom->addCoefficient(complex<cl_double>(0, 0));
	polynom->addCoefficient(complex<cl_double>(0, 0));
	polynom->addCoefficient(complex<cl_double>(1, 0));

	derivation = new Polynom((*polynom));
	derivation->differentiate();
	String polyS = polynom->printPolynom();
	derivation->printPolynom();

	res = new cl_int[2];
	res[0] = core->getXRes() * 2;
	res[1] = core->getYRes() * 2;

	ratio = (double)res[1] / (double)res[0];

	zoom = new cl_double[2];
	zoom[0] = 10;
	zoom[1] = 10 * ratio;

	centerCL = new cl_double[2];
	centerCL[0] = 0;
	centerCL[1] = 0;

	fraktal = new Image(res[0], res[1]);

	genCL = new NewtonFraktalCLGeneration();

	scene = new Scene(Scene::SCENE_2D);
	scene->getDefaultCamera()->setOrthoSize(res[0], res[1]);

	ui = new Scene(Scene::SCENE_2D_TOPLEFT);
	ui->getDefaultCamera()->setOrthoSize(core->getXRes(), core->getYRes());
	ui->rootEntity.processInputEvents = true;
	
	treeCont = new UITreeContainer("UIThemes/OpenCLIcon.png", "OpenCL Devices", 500, core->getYRes());
	ui->addChild(treeCont);
	treeCont->getRootNode()->addEventListener(this, UITreeEvent::EXECUTED_EVENT);
	
	for (int i = 0; i < genCL->platformStrs.size(); i++){
		UITree* plat = treeCont->getRootNode()->addTreeChild(Services()->getConfig()->getStringValue("Polycode","uiFileBrowserFolderIcon"), genCL->platformStrs[i], (void*)i);
		for (int j = 0; j < genCL->deviceStrs[i].size(); j++){
			plat->addTreeChild("UIThemes/DeviceIcon.png", genCL->deviceStrs[i][j], (void*)j);
		}
		plat->toggleCollapsed();
	}
	treeCont->getRootNode()->toggleCollapsed();

	treeCont->getRootNode()->addTreeChild("UIThemes/CpuIcon.png", "No OpenCL (SLOW!! Consider installing an OpenCL Driver!)", (void*)-1);
	
	Image *centerSelector = new Image(20, 20);
	centerSelector->fill(Color(0.0, 0.0, 0.0, 0.0));
	centerSelector->drawLine(0, 10, 20, 10, Color(1.0, 0.0, 0.0, 1.0));
	centerSelector->drawLine(10, 0, 10, 20, Color(1.0, 0.0, 0.0, 1.0));

	centerSel = new SceneImage(centerSelector);
	centerSel->blendingMode = Renderer::BLEND_MODE_NORMAL;
	centerSel->visible = false;
	centerSel->setPosition(core->getXRes() / 2, core->getYRes() / 2);
	ui->addEntity(centerSel);

	Image *zoomSelector = new Image(20, 20);
	zoomSelector->fill(Color(0.0, 0.0, 0.0, 0.0));
	zoomSelector->drawLine(0, 0, zoomSelector->getWidth(), 0, Color(1.0, 0.0, 0.0, 1.0));
	zoomSelector->drawLine(0, 0, 0, zoomSelector->getHeight(), Color(1.0, 0.0, 0.0, 1.0));
	zoomSelector->drawLine(zoomSelector->getWidth(), 0, 0, zoomSelector->getHeight(), Color(1.0, 0.0, 0.0, 1.0));
	zoomSelector->drawLine(0, zoomSelector->getHeight(), zoomSelector->getWidth(), 0, Color(1.0, 0.0, 0.0, 1.0));

	zoomSel = new SceneImage(zoomSelector);
	zoomSel->blendingMode = Renderer::BLEND_MODE_NORMAL;
	zoomSel->visible = false;
	zoomSel->setPosition(core->getXRes() / 2 - zoomSel->getWidth(), core->getYRes() / 2 - zoomSel->getHeight());
	ui->addEntity(zoomSel);

	win = new UIWindow("Redraw Options", 400, 300);
	win->setPosition(core->getXRes() / 2 - win->getWidth() / 2, core->getYRes() / 2 - win->getHeight() / 2);
	ui->addChild(win);
	win->enabled = false;
	win->visible = false;

	polyLabel = new UILabel("Enter your Polynom:", 12);
	win->addChild(polyLabel);
	polyLabel->setPosition(12, 20);

	polynomInput = new UITextInput(false, 390, 15);
	polynomInput->setText(polyS);
	win->addChild(polynomInput);
	polynomInput->setPosition(12, 40);

	center = new UILabel("Center:", 12);
	win->addChild(center);
	center->setPosition(12, 70);

	centerX = new UITextInput(false, 390, 15);
	win->addChild(centerX);
	centerX->setText("0");
	centerX->setNumberOnly(true);
	centerX->setPosition(12, 95);
	centerX->addEventListener(this, UIEvent::CHANGE_EVENT);

	centerY = new UITextInput(false, 390, 15);
	win->addChild(centerY);
	centerY->setText("0");
	centerY->setNumberOnly(true);
	centerY->setPosition(12, 120);
	centerY->addEventListener(this, UIEvent::CHANGE_EVENT);

	redrawWinButton = new UIButton("Draw", 40);
	win->addChild(redrawWinButton);
	redrawWinButton->setPosition(12, win->getHeight() - 40);
	redrawWinButton->addEventListener(this, UIEvent::CLICK_EVENT);

	redraw = new UIButton("Redraw", 60);
	redraw->setPositionX(85);
	redraw->addEventListener(this, UIEvent::CLICK_EVENT);
	//ui->addChild(redraw);
	redraw->enabled = false;
	redraw->visible = false;

	openOptions = new UIButton("Options", 60);
	//openOptions->setPositionX(155);
	openOptions->addEventListener(this, UIEvent::CLICK_EVENT);
	ui->addChild(openOptions);
	openOptions->enabled = false;
	openOptions->visible = false;

	zoomL = new UILabel(L"Ausschnittsgröße:", 12);
	zoomL->setPosition(12, 160);
	win->addChild(zoomL);

	zoomField = new UITextInput(false, 70, 15);
	zoomField->setNumberOnly(true);
	zoomField->setPosition(12, 180);
	win->addChild(zoomField);
	zoomField->enabled = false;
	zoomField->visible = false;

	dragging = false;
}

NewtonFraktalApp::~NewtonFraktalApp() { }

bool NewtonFraktalApp::Update() {
	if (dragging){
		double size = max(abs(Services()->getInput()->mousePosition.x - startPoint.x), abs(Services()->getInput()->mousePosition.y - startPoint.y));
		
		Image *zoomSelector = new Image(size, size * ratio);
		zoomSelector->fill(Color(0.0, 0.0, 0.0, 0.0));
		zoomSelector->fillRect(0, 0, zoomSelector->getWidth(), 3, Color(1.0, 0.0, 0.0, 1.0));
		zoomSelector->fillRect(0, 0, 3, zoomSelector->getHeight(), Color(1.0, 0.0, 0.0, 1.0));
		zoomSelector->fillRect(zoomSelector->getWidth()-3, 0, 3, zoomSelector->getHeight(), Color(1.0, 0.0, 0.0, 1.0));
		zoomSelector->fillRect(0, zoomSelector->getHeight()-3, zoomSelector->getWidth(), 3, Color(1.0, 0.0, 0.0, 1.0));

		ui->removeEntity(zoomSel);
		delete zoomSel;

		zoomSel = new SceneImage(zoomSelector);
		zoomSel->blendingMode = Renderer::BLEND_MODE_NORMAL;
		zoomSel->visible = true;
		if (Services()->getInput()->mousePosition.x - startPoint.x > 0 || Services()->getInput()->mousePosition.y - startPoint.y > 0){
			zoomSel->setPosition(startPoint.x + zoomSel->getWidth() / 2, startPoint.y + zoomSel->getHeight() / 2);
		} else {
			zoomSel->setPosition(startPoint.x - zoomSel->getWidth() / 2, startPoint.y - zoomSel->getHeight() / 2);
		}

		ui->addEntity(zoomSel);
	}
	return core->updateAndRender();
}

void NewtonFraktalApp::drawFractal(){
	const int xRes = res[0];
	const int yRes = res[1];
	bool cl;

	std::vector<double> result, iterations;
	std::vector<int> typeRes;
	if (genCL && genCL->err == CL_SUCCESS){
		cl = true;
	} else {
		cl = false;
		findZeros();
		runNewton(result, iterations, typeRes);
	}

	clock_t end = clock();

	double maxIters = 0, minIters = 0, max = 0, min = 0;
	for (int y = 0; y < fraktal->getHeight(); y++){
		for (int x = 0; x < fraktal->getWidth(); x++){
			if (cl){
				if (genCL->typeRes[x + y * xRes] < 20){
					maxIters = MAX(maxIters, genCL->iterations[x + y * xRes]);
					minIters = MIN(minIters, genCL->iterations[x + y * xRes]);
					max = MAX(max, genCL->result[x + y * xRes]);
					min = MIN(min, genCL->result[x + y * xRes]);
				}
			} else {
				if (typeRes[x + y * xRes] < 20){
					maxIters = MAX(maxIters, iterations[x + y * xRes]);
					minIters = MIN(minIters, iterations[x + y * xRes]);
					max = MAX(max, result[x + y * xRes]);
					min = MIN(min, result[x + y * xRes]);
				}

			}
		}
	}

	maxIters = maxIters / ((double)polynom->getNumCoefficients() / ((double)polynom->getNumCoefficients()/3.0));
	Logger::log("x: %f, y: %f\n", (double)(zoom[0] * ((double)0 - ((double)res[0] / 2.0)) + centerCL[0]) / (res[0]), (double)(zoom[1] * ((double)0 - ((double)res[1] / 2.0)) + centerCL[1]) / (res[1]));
	Logger::log("x: %f, y: %f\n", (double)(zoom[0] * ((double)res[0] - ((double)res[0] / 2.0)) + centerCL[0]) / (res[0]), (double)(zoom[1] * ((double)res[1] - ((double)res[1] / 2.0)) + centerCL[1]) / (res[1]));
	for (int y = 0; y < fraktal->getHeight(); y++){
		for (int x = 0; x < fraktal->getWidth(); x++){
			int type;
			double conDiv, it;
			if (cl){
				conDiv = genCL->result[x + y * xRes];
				type = genCL->typeRes[x + y * xRes];
				it = genCL->iterations[x + y * xRes];
			} else {
				conDiv = result[x + y * xRes];
				type = typeRes[x + y * xRes];
				it = iterations[x + y * xRes];
			}
			
			if (type < 20){
				Color col;
				col.setColorHexRGB(colors[type]);
				conDiv = conDiv + (double)it;
				if (conDiv > maxIters)
					conDiv = maxIters;
				if (conDiv < 0.3)
					conDiv = 0.3;
				col.setColorHSV(col.getHue(), col.getSaturation(), (1.0 - mapCL(conDiv, 0, maxIters, 0.0, 1.0)));
				fraktal->setPixel(x, y, col);
			} else {
				fraktal->setPixel(x, y, 0, 0, 0, 0);
			}
		}
	}

	FILE* logFile;
	fopen_s(&logFile, "polynoms.log", "a");
	String timeS = String::IntToString(time(NULL)), polynomS = polynom->printPolynom();
	fprintf(logFile, "Time: %s, Polynom: %s, Center: %f, %f, Zoom: %d, The computation took: %f seconds\n", timeS.c_str(), polynomS.c_str(), centerCL[0], centerCL[1], zoom[0], double(end - begin) / CLOCKS_PER_SEC);
	fraktal->saveImage(timeS + " " + String::IntToString(zoom[0])  +".png");
	fclose(logFile);

	scene->removeEntity(sceneFraktal);
	delete sceneFraktal;
	sceneFraktal = new SceneImage(fraktal);
	scene->addChild(sceneFraktal);
}

void NewtonFraktalApp::findZeros(){
	std::vector<std::complex<cl_double>> zerosT;
	cl_double x = -400, y = -400;
	while (y <= 400){
		while (x <= 400){
			std::complex<cl_double> z(x, y);
			std::complex<cl_double> zo(0, 0);
			for (int i = 0; i < 3000; i++){
				zo = z;
				z = z - polynom->getValue(z) / derivation->getValue(z);
				if (fabs(z.real() - zo.real()) <= 0.00000001 && fabs(z.imag() - zo.imag()) <= 0.00000001){
					zerosT.push_back(z);
					break;
				}
				if (z.real() > 10000){
					z = complex<double>(10000, 10000);
					break;
				}
			}
			x++;
		}
		y++;
	}
	
	for (int i = 0; i < zerosT.size(); i++){
		if (zerosT[i].real() == 10000 && zerosT[i].imag() == 10000)
			continue;

		int s = zeros.size();
		for (int j = 0; j <= s; j++){
			if (j == zeros.size()){
				zeros.push_back(zerosT[i]);
			} else if (compComplex(zeros[j], zerosT[i], 0.00000001)){
				break;
			}
		}
		if (zeros.size() == polynom->getNumCoefficients()-1)
			break;
	}
}

void NewtonFraktalApp::runNewton(std::vector<double> &result, std::vector<double> &iterations, std::vector<int> &typeRes){
	std::complex<double> z, zo, p, d;
	
	for (int y = -res[1] / 2; y < res[1] / 2; y++){
		for (int x = -res[0] / 2; x < res[0] / 2; x++){
			//z = complex<double>(x / zoom[0] + centerCL[0], y / zoom[1] + centerCL[1]);
			z = complex<double>((double)(zoom[0] * ((double)x - ((double)res[0] / 2.0)) + centerCL[0]), (double)(zoom[1] * ((double)y - ((double)res[1] / 2.0)) + centerCL[1]));
			int i = 0;
			bool found = false;
			while (i < 6000 && abs(z) < 100000 && !found){
				zo = z;
				z = z - polynom->getValue(z)/derivation->getValue(z);

				for (int j = 0; j < zeros.size(); j++){
					if (compComplex(z, zeros[j], 0.00000001)){
						typeRes.push_back(j);
						result.push_back((log(0.00000001) - log(abs(zo- zeros[j]))) / (log(abs(z- zeros[j])) - log(abs(zo-zeros[j]))));
						found = true;
						break;
					}
				}

				if (compComplex(z, zo, 0.000000000000001) && !found){
					typeRes.push_back(15);
					result.push_back(0);
					break;
				}
			}
			if (!found){
				result.push_back(0);
				typeRes.push_back(15);
			}
			iterations.push_back(i);
		}
	}
}

cl_double mapCL(cl_double x, cl_double in_min, cl_double in_max, cl_double out_min, cl_double out_max){
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void NewtonFraktalApp::handleEvent(Event* e){
	
	if (e->getDispatcher() == treeCont->getRootNode() && e->getEventCode() == UITreeEvent::EXECUTED_EVENT){
		begin = clock();

		if ((int)treeCont->getRootNode()->getSelectedNode()->getUserData() >= 0){
			cl_int paramc[] = { polynom->getNumCoefficients(), derivation->getNumCoefficients() };
			genCL->initCLAndRunNewton(zoom, res, polynom->getCLCoefficients(), derivation->getCLCoefficients(), paramc, (int)treeCont->getRootNode()->getSelectedNode()->getParent()->getUserData(), (int)treeCont->getRootNode()->getSelectedNode()->getUserData());
		}

		ui->removeEntity(treeCont);

		redraw->enabled = true;
		redraw->visible = true;
		openOptions->enabled = true;
		openOptions->visible = true;
		zoomField->enabled = true;
		zoomField->visible = true;

		drawFractal();
		
#ifdef _WINDOWS
		MSG Msg;
		for (int i = 0; i < 100; i++){ PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE); }
#endif

		core->getInput()->addEventListener(this, InputEvent::EVENT_MOUSEUP);
		core->getInput()->addEventListener(this, InputEvent::EVENT_MOUSEDOWN);
		core->getInput()->addEventListener(this, InputEvent::EVENT_KEYDOWN);
		
		clOptionsSet = true;
	}
	
	if (!clOptionsSet)
		return;

	if (e->getDispatcher() == redraw){
		redrawIt();
	} else if (e->getDispatcher() == redrawWinButton){
		redrawIt();
	} else if (e->getDispatcher() == openOptions){
		win->showWindow();
	} else if (e->getDispatcher() == centerX || e->getDispatcher() == centerY){
		centerDirty = true;
	} else if (e->getDispatcher() == core->getInput()) {
		InputEvent* ie = (InputEvent*)e;
		if (e->getEventCode() == InputEvent::EVENT_MOUSEUP){
			if (!redraw->mouseOver && /*!zoomField->getMouseOver() &&*/ !win->mouseOver && !openOptions->mouseOver && clOptionsSet){
				//	centerSel->setPosition(ie->getMousePosition().x, ie->getMousePosition().y);
				//	centerX->setText(String::NumberToString(mapCL((cl_double)centerSel->getPosition().x, 0, core->getXRes(), -(res[0]) / 2, (res[0]) / 2) / zoom[0] + this->centerCL[0], 6));
				//	centerY->setText(String::NumberToString(mapCL((cl_double)centerSel->getPosition().y, 0, core->getYRes(), -(res[1]) / 2, (res[1]) / 2) / zoom[1] + this->centerCL[1], 6));
				//	centerSel->visible = true;
				dragging = false;
				centerX->setText(String::NumberToString(mapCL((cl_double)zoomSel->getPosition().x, 0, core->getXRes(), -(zoom[0]) / 2, (zoom[0]) / 2) + this->centerCL[0], 6));
				centerY->setText(String::NumberToString(mapCL((cl_double)zoomSel->getPosition().y, 0, core->getYRes(), -(zoom[1]) / 2, (zoom[1]) / 2) + this->centerCL[1], 6));
				zoomField->setText(String::NumberToString(mapCL((cl_double)zoomSel->getWidth(), 0, core->getXRes(), 0, zoom[0]), 6));
			}
		} else if (e->getEventCode() == InputEvent::EVENT_MOUSEDOWN){
			if (!redraw->mouseOver && !win->mouseOver && !openOptions->mouseOver && clOptionsSet){
				dragging = true;
				startPoint = ie->getMousePosition();
			}
		} else if (e->getEventCode() == InputEvent::EVENT_KEYDOWN){
			switch (ie->key){
			case PolyKEY::KEY_RETURN:
				redrawIt();
				break;
			default:
				break;
			}
		}
	} 
}

void NewtonFraktalApp::redrawIt(){
	polynom = Polynom::readFromString(polynomInput->getText());

	derivation = new Polynom((*polynom));
	derivation->differentiate();

	polynom->printPolynom();
	derivation->printPolynom();

	if (zoomField->getText().isNumber()){
		zoom[0] = zoomField->getText().toNumber();
		zoom[1] = zoomField->getText().toNumber() * ratio;
	}
	cl_double *center = new cl_double[2];
//	center[0] = mapCL((cl_double)centerSel->getPosition().x, 0, core->getXRes(), -(res[0]) / 2, (res[0]) / 2) / zoom[0] + this->centerCL[0];
//	center[1] = mapCL((cl_double)centerSel->getPosition().y, 0, core->getYRes(), -(res[1]) / 2, (res[1]) / 2) / zoom[1] + this->centerCL[1];

	//center[0] = mapCL((cl_double)zoomSel->getPosition().x, 0, core->getXRes(), -(res[0]) / 2, (res[0]) / 2) / zoom[0] + this->centerCL[0];
	//center[1] = mapCL((cl_double)zoomSel->getPosition().y, 0, core->getYRes(), -(res[1]) / 2, (res[1]) / 2) / zoom[1] + this->centerCL[1];

	//if (centerDirty){
		if (centerX->getText().isNumber()){
			center[0] = centerX->getText().toNumber();
		}
		if (centerY->getText().isNumber()){
			center[1] = centerY->getText().toNumber();
		}
		centerDirty = false;
	//}

	begin = clock();

	cl_int paramc[] = { polynom->getNumCoefficients(), derivation->getNumCoefficients() };

	genCL->runNewton(zoom, res, center, polynom->getCLCoefficients(), derivation->getCLCoefficients(), paramc);

	this->centerCL = center;
	drawFractal();

	centerSel->visible = false;
	centerSel->setPosition(((core->getXRes() / 2) - center[0]), (core->getYRes() / 2) - center[1]);
	zoomSel->visible = false;
}