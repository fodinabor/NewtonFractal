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
	0xff9900,
	0xff5500,
	0x55ff00,
	0x00ff55,
	0x0055ff
};

NewtonFraktalApp::NewtonFraktalApp(PolycodeView *view) {
	core = new POLYCODE_CORE(view, 1500, 1000, false, false, 0, 0, 20);
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

	//Performance Test Polynom
	//polynom->addCoefficient(complex<cl_double>(-1, 0));
	//polynom->addCoefficient(complex<cl_double>(0, 1));
	//polynom->addCoefficient(complex<cl_double>(1, 1));
	//polynom->addCoefficient(complex<cl_double>(1, 0));
	//polynom->addCoefficient(complex<cl_double>(1, 0));
	//polynom->addCoefficient(complex<cl_double>(1, 9));
	//polynom->addCoefficient(complex<cl_double>(3,1));
	//polynom->addCoefficient(complex<cl_double>(1, 9));
	//polynom->addCoefficient(complex<cl_double>(2, 5));
	//polynom->addCoefficient(complex<cl_double>(1, 3));
	//polynom->addCoefficient(complex<cl_double>(2, 1));
	//polynom->addCoefficient(complex<cl_double>(3, 2));
	//polynom->addCoefficient(complex<cl_double>(9, 2));
	//polynom->addCoefficient(complex<cl_double>(12, 3));
	//polynom->addCoefficient(complex<cl_double>(3, 9));
	//polynom->addCoefficient(complex<cl_double>(0, 1));
	//polynom->addCoefficient(complex<cl_double>(7, 1));
	//polynom->addCoefficient(complex<cl_double>(2, 4));
	//polynom->addCoefficient(complex<cl_double>(4, 3));
	//polynom->addCoefficient(complex<cl_double>(1, 0));

	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));
	//polynom->addCoefficient(complex<cl_double>(rand() % 15, rand() % 15));

	//polynom = Polynom::readFromString("( 1 + 6 i)*x^ 23 + ( 14 + 3 i)*x^ 22 + ( 3 + 2 i)*x^ 21 + ( 6 + 8 i)*x^ 20 + ( 9 + 10 i)*x^ 19 + ( 4 + 2 i)*x^ 18 + ( 7 + 9 i)*x^ 17 + ( 3 + 11 i)*x^ 16 + ( 6 + 2 i)*x^ 15 + ( 5 + 8 i)*x^ 14 + ( 11 + 6 i)*x^ 13 + ( 7 + 7 i)*x^ 12 + ( 3 + 2 i)*x^ 11 + ( 9 + 6 i)*x^ 10 + ( 6 + 12 i)*x^ 9 + ( 2 + 10 i)*x^ 8 + ( 11 + 1 i)*x^ 7 + ( 12 + 1 i)*x^ 6 + ( 5 + 5 i)*x^ 5 + ( 14 + 7 i)*x^ 4 + ( 3 + 3 i)*x^ 3 + ( 4 + 14 i)*x^ 2 + ( 10 + 4 i)*x + ( 2 + 11 i)");

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

	selScene = new Scene(Scene::SCENE_2D_TOPLEFT);
	selScene->getDefaultCamera()->setOrthoSize(core->getXRes(), core->getYRes());

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
	selScene->addEntity(zoomSel);

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
	redraw->enabled = false;
	redraw->visible = false;

	openOptions = new UIButton("Options", 60);
	openOptions->addEventListener(this, UIEvent::CLICK_EVENT);
	ui->addChild(openOptions);
	openOptions->enabled = false;
	openOptions->visible = false;

	zoomL = new UILabel(L"Ausschnittsgröße:", 12);
	zoomL->setPosition(12, 160);
	win->addChild(zoomL);

	zoomField = new UITextInput(false, 390, 15);
	zoomField->setNumberOnly(true);
	zoomField->setPosition(12, 180);
	win->addChild(zoomField);
	zoomField->enabled = false;
	zoomField->visible = false;
	zoomField->setText(String::NumberToString(zoom[0]));
	zoomField->addEventListener(this, UIEvent::CHANGE_EVENT);

	dragging = false;
	useCPU = true;
}

NewtonFraktalApp::~NewtonFraktalApp() { }

bool NewtonFraktalApp::Update() {
	if (dragging || (centerDirty && zoomSel->visible)) {
		double size;
		if (dragging) {
			size = max(abs(Services()->getInput()->mousePosition.x - startPoint.x), abs(Services()->getInput()->mousePosition.y - startPoint.y));
		} else if (centerDirty) {
			size = mapCL((cl_double)zoomField->getText().toNumber(), 0, zoom[0], 0, core->getXRes());
		}

		if (size * (size * ratio) * 3 < MAXINT) {
			Image *zoomSelector = new Image(size, size * ratio);
			zoomSelector->fill(Color(0.0, 0.0, 0.0, 0.0));
			zoomSelector->fillRect(0, 0, zoomSelector->getWidth(), 3, Color(1.0, 0.0, 0.0, 0.7));
			zoomSelector->fillRect(0, 0, 3, zoomSelector->getHeight(), Color(1.0, 0.0, 0.0, 0.7));
			zoomSelector->fillRect(zoomSelector->getWidth() - 3, 0, 3, zoomSelector->getHeight(), Color(1.0, 0.0, 0.0, 0.7));
			zoomSelector->fillRect(0, zoomSelector->getHeight() - 3, zoomSelector->getWidth(), 3, Color(1.0, 0.0, 0.0, 0.7));

			selScene->removeEntity(zoomSel);
			delete zoomSel;

			zoomSel = new SceneImage(zoomSelector);
			zoomSel->blendingMode = Renderer::BLEND_MODE_NORMAL;
			zoomSel->visible = true;
			selScene->addEntity(zoomSel);

			if (dragging) {
				if (Services()->getInput()->mousePosition.x - startPoint.x > 0) {
					zoomSel->setPositionX(startPoint.x + zoomSel->getWidth() / 2);
				} else {
					zoomSel->setPositionX(startPoint.x - zoomSel->getWidth() / 2);
				}

				if (Services()->getInput()->mousePosition.y - startPoint.y > 0) {
					zoomSel->setPositionY(startPoint.y + zoomSel->getHeight() / 2);
				} else {
					zoomSel->setPositionY(startPoint.y - zoomSel->getHeight() / 2);
				}
			} else if (centerDirty) {
				double x = mapCL((cl_double)centerX->getText().toNumber() - this->centerCL[0], -(zoom[0]) / 2, (zoom[0]) / 2, 0, core->getXRes());
				double y = mapCL((cl_double)-(centerY->getText().toNumber() - this->centerCL[1]), -(zoom[1]) / 2, (zoom[1]) / 2, 0, core->getYRes());
				zoomSel->setPosition(x, y);
				centerDirty = false;
			}
		}
	}

	bool update = core->updateAndRender();
	
	if (clOptionsJustSet) {
		core->getInput()->addEventListener(this, InputEvent::EVENT_MOUSEUP);
		core->getInput()->addEventListener(this, InputEvent::EVENT_MOUSEDOWN);
		core->getInput()->addEventListener(this, InputEvent::EVENT_KEYDOWN);
		clOptionsJustSet = false;
	}
	
	return update;
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

	double maxIters = 0;
	for (int y = 0; y < fraktal->getHeight(); y++){
		for (int x = 0; x < fraktal->getWidth(); x++){
			if (cl){
				if (genCL->typeRes[x + y * xRes] >= 0){
					maxIters = MAX(maxIters, genCL->iterations[x + y * xRes]);
				}
			} else {
				if (typeRes[x + y * xRes] >= 0){
					maxIters = MAX(maxIters, iterations[x + y * xRes]);
				}
			}
		}
	}

	maxIters = maxIters / ((double)polynom->getNumCoefficients() / ((double)polynom->getNumCoefficients()/3.0));
	
	for (int y = 0; y < yRes; y++){
		for (int x = 0; x < xRes; x++){
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
			
			if (type >= 0){
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
				if(type == -1)
					fraktal->setPixel(x, y, 1, 1, 1, 1);
				else 
					fraktal->setPixel(x, y, 1, 1, 0, 1);
			}
		}
	}

	FILE* logFile;
	fopen_s(&logFile, "polynoms.log", "a");
	String timeS = String::IntToString(time(NULL)), polynomS = polynom->printPolynom();
	fprintf(logFile, "Time: %s, Polynom: %s, Center: %f, %f, Area size: x: %f y: %f, MaxIters: %f, The computation took: %s seconds\n", timeS.c_str(), polynomS.c_str(), centerCL[0], centerCL[1], zoom[0], zoom[1], maxIters, String::NumberToString(double(end - begin) / CLOCKS_PER_SEC).c_str());
	fraktal->saveImage(timeS + ".png");
	fclose(logFile);

	if(sceneFraktal)
		Services()->getMaterialManager()->deleteTexture(sceneFraktal->getTexture());
	scene->removeEntity(sceneFraktal);
	delete sceneFraktal;
	sceneFraktal = new SceneImage(fraktal);
	scene->addChild(sceneFraktal);

	if (cl) {
		//free(genCL->result);
		//free(genCL->typeRes);
		//free(genCL->iterations);
	} else {
		result.clear();
		typeRes.clear();
		iterations.clear();
	}
}

void NewtonFraktalApp::findZeros(){
	std::vector<std::complex<cl_double>> zerosT;
	for (int y = -200; y < 200; y++) {
		for (int x = -200; x < 200; x++) {
			std::complex<cl_double> z(x, y);
			std::complex<cl_double> zo(0, 0);
			for (int i = 0; i < 1000; i++) {
				zo = z;
				z = z - polynom->getValue(z) / derivation->getValue(z);
				if (compComplex(z, zo, RESOLUTION)) {
					zerosT.push_back(z);
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
				Logger::log("Zero %d: (%f|%f)\n", j, this->zeros[j].real(), this->zeros[j].imag());
			} else if (compComplex(zeros[j], zerosT[i], RESOLUTION)) {
				break;
			}
		}
		if (zeros.size() == polynom->getNumCoefficients()-1)
			break;
	}
}

void NewtonFraktalApp::runNewton(std::vector<double> &result, std::vector<double> &iterations, std::vector<int> &typeRes){
	std::complex<double> z, zo, p, d;
	const int xRes = res[0];
	const int yRes = res[1];
	
	for (int y = 0; y < res[1]; y++){
		for (int x = 0; x < res[0]; x++){
			const double a = (double)zoom[0] * ((double)x - ((double)xRes / 2.0)) / xRes + centerCL[0];
			const double b = (double)zoom[1] * (((double)yRes / 2.0) - (double)y) / yRes + centerCL[1];

			z = complex<double>(a, b);
			typeRes.push_back(-1);
			result.push_back(0);

			bool found = false;
			int i = 0;
			while (i < 6000 && abs(z) < 100000 && !found) {
				p = polynom->getValue(z);
				d = derivation->getValue(z);
				
				zo = z;

				z = z - p / d;

				i++;

				for (int j = 0; j < polynom->getNumCoefficients() - 1; j++) {
					if (compComplex(z, zeros[j], RESOLUTION)) {
						typeRes[typeRes.size()-1] = j;
						result[x + y * res[0]] = (log(RESOLUTION) - log(abs(zo - zeros[j]))) / (log(abs(z - zeros[j])) - log(abs(zo - zeros[j])));
						found = true;
						break;
					}
				}

				if (compComplex(z, zo, RESOLUTION / 10) && !found) {
					typeRes[typeRes.size()-1] = -1;
					break;
				}
			}
			if (abs(z) >= 10000) {
				typeRes[typeRes.size()-1] = -1;
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
			useCPU = false;
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
		while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		Update();
#endif

		clOptionsSet = true;
		clOptionsJustSet = true;
	}
	
	if (!clOptionsSet)
		return;

	if (e->getDispatcher() == redraw){
		redrawIt();
	} else if (e->getDispatcher() == redrawWinButton){
		redrawIt();
	} else if (e->getDispatcher() == openOptions){
		win->showWindow();
	} else if ((e->getDispatcher() == centerX || e->getDispatcher() == centerY || e->getDispatcher()==zoomField)) {
		centerDirty = true;
	} else if (e->getDispatcher() == core->getInput()) {
		InputEvent* ie = (InputEvent*)e;
		if (e->getEventCode() == InputEvent::EVENT_MOUSEUP){
			if (!redraw->mouseOver && !win->mouseOver && !openOptions->mouseOver && clOptionsSet){
				centerX->setText(String::NumberToString(mapCL((cl_double)zoomSel->getPosition().x, 0, core->getXRes(), -(zoom[0]) / 2, (zoom[0]) / 2) + this->centerCL[0], 6),false);
				centerY->setText(String::NumberToString(-mapCL((cl_double)zoomSel->getPosition().y, 0, core->getYRes(), -(zoom[1]) / 2, (zoom[1]) / 2) + this->centerCL[1], 6), false);
				zoomField->setText(String::NumberToString(mapCL((cl_double)zoomSel->getWidth(), 0, core->getXRes(), 0, zoom[0]), 6), false);
				dragging = false;
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
		if (centerY->getText().isNumber()) {
			center[1] = centerY->getText().toNumber();
		}
	//}

	begin = clock();
	if (!useCPU) {
		cl_int paramc[] = { polynom->getNumCoefficients(), derivation->getNumCoefficients() };
		genCL->runNewton(zoom, res, center, polynom->getCLCoefficients(), derivation->getCLCoefficients(), paramc);
	}

	this->centerCL = center;
	drawFractal();

	centerSel->visible = false;
	centerSel->setPosition(((core->getXRes() / 2) - center[0]), (core->getYRes() / 2) - center[1]);
	zoomSel->visible = false;
}