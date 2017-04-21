/*
The MIT License (MIT)

Copyright (c) 2016 By Joachim Meyer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "NewtonFraktalApp.h"
#include "NewtonFraktal.h"
#include "NewtonFraktalGeneration.h"
#include <ctime>
#include <time.h>
#include <cmath>

#define pi 3.14159265359

std::vector<int> colorsHex = {
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

NewtonFraktalApp::NewtonFraktalApp(NewtonFraktalView *view) {
	core = new POLYCODE_CORE(view, 1000, 700, false, false, 0, 0, 20);
	//Services()->getRenderer()->setClearColor(0, 0, 0, 1);
	Services()->getRenderer()->setClearColor(Color::ColorWithHex(0x494949ff));
	core->updateAndRender();
	CoreServices::getInstance()->getResourceManager()->addArchive("default.pak");
	CoreServices::getInstance()->getResourceManager()->addDirResource("default", false);
	CoreServices::getInstance()->getResourceManager()->addArchive("UIThemes.pak");
	CoreServices::getInstance()->getConfig()->loadConfig("Polycode", "theme.xml");

	SceneLabel::defaultAnchor = Vector3(-1.0, -1.0, 0.0);
	SceneLabel::defaultPositionAtBaseline = true;
	SceneLabel::defaultSnapToPixels = true;
	SceneLabel::createMipmapsForLabels = false;

	Entity::defaultBlendingMode = Renderer::BLEND_MODE_NONE;
	CoreServices::getInstance()->getRenderer()->setTextureFilteringMode(Renderer::TEX_FILTERING_NEAREST);

	Services()->getLogger()->setLogToFile(true);

	clOptionsSet = false;

	gen = new NewtonFraktalGeneration();

	polynom = new Polynom();
	//polynom = Polynom::readFromString("( 4 + 2 i)*x^ 18 + ( 7 + 9 i)*x^ 17 + ( 3 + 11 i)*x^ 16 + ( 6 + 2 i)*x^ 15 + ( 5 + 8 i)*x^ 14 + ( 11 + 6 i)*x^ 13 + ( 7 + 7 i)*x^ 12 + ( 3 + 2 i)*x^ 11 + ( 9 + 6 i)*x^ 10 + ( 6 + 12 i)*x^ 9 + ( 2 + 10 i)*x^ 8 + ( 11 + 1 i)*x^ 7 + ( 12 + 1 i)*x^ 6 + ( 5 + 5 i)*x^ 5 + ( 14 + 7 i)*x^ 4 + ( 3 + 3 i)*x^ 3 + ( 4 + 14 i)*x^ 2 + ( 10 + 4 i)*x + ( 2 + 11 i)");
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
	res[0] = 1000;
	res[1] = 1000;

	ratio = (double)res[1] / (double)res[0];

	zoom = (double*)_aligned_malloc(2 * sizeof(double), MEM_ALIGN);
	zoom[0] = 10;
	zoom[1] = 10 * ratio;

	centerCL = (double*)_aligned_malloc(2 * sizeof(double), MEM_ALIGN);
	centerCL[0] = 0;
	centerCL[1] = 0;

	scene = new Scene(Scene::SCENE_2D);
	scene->getDefaultCamera()->setOrthoSize(res[0], res[1]);

	selScene = new Scene(Scene::SCENE_2D_TOPLEFT);
	selScene->getDefaultCamera()->setOrthoSize(core->getXRes(), core->getYRes());

	ui = new Scene(Scene::SCENE_2D_TOPLEFT);
	ui->getDefaultCamera()->setOrthoSize(core->getXRes(), core->getYRes());
	ui->rootEntity.processInputEvents = true;
	
	treeCont = new UITreeContainer("OpenCLIcon.png", "OpenCL Devices", 500, core->getYRes());
	ui->addChild(treeCont);
	treeCont->getRootNode()->addEventListener(this, UITreeEvent::EXECUTED_EVENT);
	
	std::vector<String> platforms = NewtonFraktalCLGenerator::getPlatforms();
	std::vector<std::vector<String>> devices = NewtonFraktalCLGenerator::getDevices();
	for (int i = 0; i < platforms.size(); i++){
		UITree* plat = treeCont->getRootNode()->addTreeChild(Services()->getConfig()->getStringValue("Polycode","uiFileBrowserFolderIcon"), platforms[i], (void*)i);
		for (int j = 0; j < devices[i].size(); j++){
			plat->addTreeChild("DeviceIcon.png", devices[i][j], (void*)j);
		}
		plat->toggleCollapsed();
	}
	treeCont->getRootNode()->toggleCollapsed();

	treeCont->getRootNode()->addTreeChild("CpuIcon.png", "No OpenCL (SLOW!! Consider installing an OpenCL Driver!)", (void*)-1);
	
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

	win = new UIWindow("Redraw Options", 400, 400);
	win->setPosition(core->getXRes() / 2 - win->getWidth() / 2, core->getYRes() / 2 - win->getHeight() / 2);
	ui->addChild(win);
	win->enabled = false;
	win->visible = false;

	polyLabel = new UILabel("Enter your Polynom:", 12);
	win->addChild(polyLabel);
	polyLabel->setPosition(12, 20);

	polynomInput = new UITextInput(false, 390, 15);
	polynomInput->setText(polyS);
	win->addFocusChild(polynomInput);
	polynomInput->setPosition(12, 40);

	center = new UILabel("Center:", 12);
	win->addChild(center);
	center->setPosition(12, 70);

	centerX = new UITextInput(false, 390, 15);
	win->addFocusChild(centerX);
	centerX->setText("0");
	centerX->setNumberOnly(true);
	centerX->setPosition(12, 95);
	centerX->addEventListener(this, UIEvent::CHANGE_EVENT);

	centerY = new UITextInput(false, 390, 15);
	win->addFocusChild(centerY);
	centerY->setText("0");
	centerY->setNumberOnly(true);
	centerY->setPosition(12, 120);
	centerY->addEventListener(this, UIEvent::CHANGE_EVENT);

	zoomL = new UILabel("Areasize:", 12);
	zoomL->setPosition(12, 160);
	win->addChild(zoomL);

	zoomField = new UITextInput(false, 390, 15);
	zoomField->setNumberOnly(true);
	zoomField->setPosition(12, 180);
	win->addFocusChild(zoomField);
	zoomField->enabled = false;
	zoomField->visible = false;
	zoomField->setText(String::NumberToString(zoom[0]));
	zoomField->addEventListener(this, UIEvent::CHANGE_EVENT);

	resLabel = new UILabel("Resolution:", 12);
	win->addChild(resLabel);
	resLabel->setPosition(12, 220);

	resXInput = new UITextInput(false, 50, 15);
	win->addFocusChild(resXInput);
	resXInput->setText(String::IntToString(res[0]));
	resXInput->setNumberOnly(true);
	resXInput->setPosition(12, 235);
	resXInput->addEventListener(this, UIEvent::CHANGE_EVENT);

	resTimesLabel = new UILabel("x", 12);
	win->addChild(resTimesLabel);
	resTimesLabel->setPosition(75, 240);

	resYInput = new UITextInput(false, 50, 15);
	win->addFocusChild(resYInput);
	resYInput->setText(String::IntToString(res[1]));
	resYInput->setNumberOnly(true);
	resYInput->setPosition(85, 235);
	resYInput->addEventListener(this, UIEvent::CHANGE_EVENT);

	contrastLabel = new UILabel("Contrast:", 12);
	win->addChild(contrastLabel);
	contrastLabel->setPosition(12, 260);

	contrastValue = 0.35;

	contrastSlider = new UIHSlider(0.0, 1.0, 100);
	contrastSlider->setPosition(12, 280);
	contrastSlider->setContinuous(true);
	contrastSlider->setSliderValue(contrastValue);
	win->addFocusChild(contrastSlider);
	contrastSlider->addEventListener(this, UIEvent::CHANGE_EVENT);

	contrastInput = new UITextInput(false, 270, 12);
	contrastInput->setNumberOnly(true);
	contrastInput->setPosition(120, 275);
	win->addFocusChild(contrastInput);
	contrastInput->setText(String::NumberToString(contrastValue));
	contrastInput->addEventListener(this, UIEvent::CHANGE_EVENT);

	redrawWinButton = new UIButton("Draw", 40);
	win->addChild(redrawWinButton);
	redrawWinButton->setPosition(12, win->getHeight() - 40);
	redrawWinButton->addEventListener(this, UIEvent::CLICK_EVENT);

	nonDoubleLabel = new UILabel("Warning: Only Polynoms with degree 8 supported.", 12);
	win->addChild(nonDoubleLabel);
	nonDoubleLabel->setColor(0.9, 0.5, 0.0, 1.0);
	nonDoubleLabel->visible = false;
	nonDoubleLabel->setPosition(12, 305);
	
	randomGenButton = new UIButton("Random", 50);
	win->addChild(randomGenButton);
	randomGenButton->setPosition(56, win->getHeight() - 40);
	randomGenButton->addEventListener(this, UIEvent::CLICK_EVENT);

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

	win->focusChild(NULL);
	dragging = false;
	startPoint.x = 0.0;
	startPoint.y = 0.0;

	for (int i = 0; i < colorsHex.size(); i++) {
		Color col;
		col.setColorHexRGB(colorsHex[i]);
		colors.push_back(col);
	}

	core->addEventListener(this, Core::EVENT_CORE_RESIZE);
}

NewtonFraktalApp::~NewtonFraktalApp() { }

bool NewtonFraktalApp::Update() {
	if (dragging || (centerDirty && zoomSel->visible)) {
		if (timer <= 0.1)
			timer += core->getElapsed();

		double size;
		if (dragging) {
			size = max(abs(Services()->getInput()->mousePosition.x - startPoint.x), abs(Services()->getInput()->mousePosition.y - startPoint.y));
		} else if (centerDirty) {
			size = mapCL((cl_double) zoomField->getText().toNumber(), 0, zoom[0], 0, res[0] / scene->getActiveCamera()->getOrthoSizeX() * core->getXRes());
		}

		if (size * (size * ratio) * 3 < MAXINT && size > RESOLUTION / 100) {
			Image *zoomSelector = new Image(size, size * ratio);
			zoomSelector->fill(Color(0.0, 0.0, 0.0, 0.0));
			zoomSelector->fillRect(0, 0, zoomSelector->getWidth(), 3, Color(1.0, 0.0, 0.0, 0.7));
			zoomSelector->fillRect(0, 0, 3, zoomSelector->getHeight(), Color(1.0, 0.0, 0.0, 0.7));
			zoomSelector->fillRect(zoomSelector->getWidth() - 3, 0, 3, zoomSelector->getHeight(), Color(1.0, 0.0, 0.0, 0.7));
			zoomSelector->fillRect(0, zoomSelector->getHeight() - 3, zoomSelector->getWidth(), 3, Color(1.0, 0.0, 0.0, 0.7));

			Services()->getRenderer()->destroyTexture(zoomSel->getTexture());
			selScene->removeEntity(zoomSel);
			delete zoomSel;

			zoomSel = new SceneImage(zoomSelector);
			zoomSel->blendingMode = Renderer::BLEND_MODE_NORMAL;
			zoomSel->visible = true;
			selScene->addEntity(zoomSel);
			delete zoomSelector;

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
				double x = mapCL((cl_double) centerX->getText().toNumber() - this->centerCL[0], -(zoom[0]) / 2, (zoom[0]) / 2, 0, core->getXRes());
				double y = mapCL((cl_double) -(centerY->getText().toNumber() - this->centerCL[1]), -(zoom[1]) / 2, (zoom[1]) / 2, 0, core->getYRes());
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

cl_double mapCL(cl_double x, cl_double in_min, cl_double in_max, cl_double out_min, cl_double out_max){
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void NewtonFraktalApp::handleEvent(Event* e){
	
	if (e->getDispatcher() == treeCont->getRootNode() && e->getEventCode() == UITreeEvent::EXECUTED_EVENT){
		begin = clock();

		if ((int)treeCont->getRootNode()->getSelectedNode()->getUserData() >= 0){
			NewtonFraktalCLGenerator *clGen = new NewtonFraktalCLGenerator();
			clGen->initCL((int)treeCont->getRootNode()->getSelectedNode()->getParent()->getUserData(), (int)treeCont->getRootNode()->getSelectedNode()->getUserData());
			gen->registerGenerator(clGen, gen->GENERATION_MODE_CL);

			generationMode = gen->GENERATION_MODE_CL;
		} else {
			generationMode = gen->GENERATION_MODE_CPU;
		}

		ui->removeEntity(treeCont);

		redraw->enabled = true;
		redraw->visible = true;
		openOptions->enabled = true;
		openOptions->visible = true;
		zoomField->enabled = true;
		zoomField->visible = true;

		NewtonFraktal *fraktal = new NewtonFraktal(res[0], res[1]);
		fraktal->setArea(zoom[0], zoom[1]);
		fraktal->setCenter(centerCL[0], centerCL[1]);
		fraktal->setContrast(contrastValue);
		fraktal->setPolynom(polynom);

		gen->generate(fraktal, generationMode);

		sceneFraktal = new SceneImage(fraktal);
		scene->addEntity(sceneFraktal);

		delete fraktal;

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
		polynom = Polynom::readFromString(polynomInput->getText());
		redrawIt();
	} else if (e->getDispatcher() == redrawWinButton){
		polynom = Polynom::readFromString(polynomInput->getText());
		redrawIt();
	} else if (e->getDispatcher() == openOptions) {
		win->setPosition(core->getXRes() / 2 - win->getWidth() / 2, core->getYRes() / 2 - win->getHeight() / 2);
		if (!usingDouble())
			nonDoubleLabel->visible = true;
		win->showWindow();
	} else if (e->getDispatcher() == contrastSlider) {
		contrastValue = contrastSlider->getSliderValue();
		contrastInput->setText(String::NumberToString(contrastValue, 6));
	} else if (e->getDispatcher() == contrastInput) {
		contrastValue = contrastInput->getText().toNumber();
		contrastSlider->setSliderValue(contrastValue);
	} else if (e->getDispatcher() == randomGenButton) {
		generateRandom();
	} else if ((e->getDispatcher() == resXInput || e->getDispatcher() == resYInput)) {
		if (resXInput->getText().isInteger() && resXInput->getText().toInteger() != 0) {
			res[0] = resXInput->getText().toInteger();
		}
		if (resYInput->getText().isInteger() && resYInput->getText().toInteger() != 0) {
			res[1] = resYInput->getText().toInteger();
		}
		ratio = (double) res[1] / (double) res[0];
	} else if ((e->getDispatcher() == centerX || e->getDispatcher() == centerY || e->getDispatcher() == zoomField)) {
		centerDirty = true;
	} else if (e->getDispatcher() == core) {
		switch (e->getEventCode()) {
		case Core::EVENT_CORE_RESIZE:
			double screenRatio = core->getYRes() / core->getXRes();
			ratio = (double) res[1] / (double) res[0];
			if (screenRatio > ratio) {
				scene->getActiveCamera()->setOrthoSize(res[0], res[0] * screenRatio);
			} else {
				scene->getActiveCamera()->setOrthoSize(res[0] / screenRatio, res[1]);
			}
			selScene->getActiveCamera()->setOrthoSize(core->getXRes(), core->getYRes());
			ui->getActiveCamera()->setOrthoSize(core->getXRes(), core->getYRes());
			centerDirty = true;
			break;
		}
	} else if (e->getDispatcher() == core->getInput()) {
		InputEvent* ie = (InputEvent*)e;
		if (e->getEventCode() == InputEvent::EVENT_MOUSEUP){
			if (dragging && timer > 0.1) {
				centerX->setText(String::NumberToString(mapCL((cl_double) zoomSel->getPosition().x, 0, core->getXRes(), -(zoom[0]) / 2, (zoom[0]) / 2) + this->centerCL[0], 15), false);
				centerY->setText(String::NumberToString(-mapCL((cl_double) zoomSel->getPosition().y, 0, core->getYRes(), -(zoom[1]) / 2, (zoom[1]) / 2) + this->centerCL[1], 15), false);
				zoomField->setText(String::NumberToString(mapCL((cl_double) zoomSel->getWidth(), 0, res[0] / scene->getActiveCamera()->getOrthoSizeX() * core->getXRes(), 0, zoom[0]), 15), false);
			}
			dragging = false;
		} else if (e->getEventCode() == InputEvent::EVENT_MOUSEDOWN){
			if (!redraw->mouseOver && (!win->mouseOver || !win->visible) && !openOptions->mouseOver && clOptionsSet){
				dragging = true;
				startPoint = ie->getMousePosition();
				timer = 0;
			}
		} else if (e->getEventCode() == InputEvent::EVENT_KEYDOWN){
			if (!redrawWinButton->hasFocus) {
				switch (ie->key) {
				case PolyKEY::KEY_RETURN:
					polynom = Polynom::readFromString(polynomInput->getText());
					redrawIt();
					break;
				default:
					break;
				}
			}
		}
	}
}

void NewtonFraktalApp::redrawIt(){
	derivation = new Polynom((*polynom));
	derivation->differentiate();

	polynom->printPolynom();
	derivation->printPolynom();

	if (resXInput->getText().isInteger()) {
		res[0] = resXInput->getText().toInteger();
	}
	if (resYInput->getText().isInteger()) {
		res[1] = resYInput->getText().toInteger();
	}

	ratio = (double) res[1] / (double) res[0];

	if (zoomField->getText().isNumber()){
		zoom[0] = zoomField->getText().toNumber();
		zoom[1] = zoomField->getText().toNumber() * ratio;
	}

	cl_double *center = (double*)_aligned_malloc(2 * sizeof(double), MEM_ALIGN);
	if (centerX->getText().isNumber()){
		center[0] = centerX->getText().toNumber();
	}
	if (centerY->getText().isNumber()) {
		center[1] = centerY->getText().toNumber();
	}

	if (sceneFraktal) {
		Services()->getRenderer()->destroyTexture(sceneFraktal->getTexture());
		scene->removeEntity(sceneFraktal);
		delete sceneFraktal;
	}

	begin = clock();

	this->centerCL = center;

	NewtonFraktal *fraktal = new NewtonFraktal(res[0], res[1]);
	fraktal->setArea(zoom[0], zoom[1]);
	fraktal->setCenter(centerCL[0], centerCL[1]);
	fraktal->setContrast(contrastValue);
	fraktal->setPolynom(polynom);

	gen->generate(fraktal, generationMode);

	sceneFraktal = new SceneImage(fraktal);
	scene->addEntity(sceneFraktal);

	delete fraktal;

	centerSel->visible = false;
	centerSel->setPosition(((core->getXRes() / 2) - center[0]), (core->getYRes() / 2) - center[1]);
	zoomSel->visible = false;
}

void NewtonFraktalApp::generateRandom() {
	int degree;
	if (usingDouble())
		degree = (rand() % (MAX_DEGREE - 2)) + 3;
	else
		degree = (rand() % (MAX_DEGREE_FLOAT - 2)) + 3;
	
	polynom->clear();
	for (int i = 0; i <= degree; i++) {
		polynom->addCoefficient(complex<cl_double>(rand() % 20, rand() % 20));
	}

	polynomInput->setText(polynom->getString());

	redrawIt();
}
