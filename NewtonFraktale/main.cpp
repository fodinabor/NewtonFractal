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
#include "NewtonFraktalGlobals.h"
#include "NewtonFraktalView.h"
#include "windows.h"
#include "resource.h"
#include "NewtonFraktalApp.h"
#include "CPUID.h"
#include <iostream>

const InstructionSet::InstructionSet_Internal InstructionSet::CPU_Rep;
extern void OpenConsole();

void OutputUsage(String name) {
	OpenConsole();
	cout << "Usage: " << name.contents << " [-q] [-d] [-v fps length]" << endl;
	cout << "\tq:\tquiet - does not open the interface. Generates random fraktal." << endl;
	cout << "\td:\tdebug - opens debug console." << endl;
	cout << "\tv:\tvideo - generates a video from a random fraktal.\n\t\tLength and FPS indicate how long the video should be in sec and how many frames per second should be generated." << endl;
	cout << "Bitte Enter drücken..." << endl;
	getchar();
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	bool debug = false, createWindow = true, vid = false;
	int framerate = 20, duration = 5;
	int nArgs;
	LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	for (int i = 1; i < nArgs; i++) {
		String arg = String(szArglist[i]);
		if (arg == "-d" || arg == "/d")
			debug = true;
		else if (arg == "-q" || arg == "/q")
			createWindow = false;
		else if (arg == "-v" || arg == "/v") {
			createWindow = false;
			vid = true;
			if (nArgs > i + 1)
				framerate = String(szArglist[++i]).toInteger();
			else
				framerate = 20;
			if (nArgs > i + 1)
				duration = String(szArglist[++i]).toInteger();
			else
				duration = 5;
		} else if (arg == "?" || arg == "/?" || arg == "-?" || arg == "--help"){
			OutputUsage(szArglist[0]);
			return 0;
		} else {
			OutputUsage(szArglist[0]);
			return 1;
		}
	}

	srand(time(NULL));

	Logger::getInstance()->setLogToFile(true);
	Logger::log("Using SSE2: %s, AVX: %s.\n", InstructionSet::SSE2() ? "yes" : "no", InstructionSet::AVX() ? "yes" : "no");

#ifdef _DEBUG
	debug = true;
#endif
	if (createWindow) {
		POINT minSize;
		minSize.x = 500;
		minSize.y = 500;

		NewtonFraktalView *view = new NewtonFraktalView(hInstance, nCmdShow, L"NewtonFraktale", minSize, true, debug);
		NewtonFraktalApp *app = new NewtonFraktalApp(view);

		MSG Msg;
		do {
			while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}
		} while (app->Update());

		delete app;
		delete view;

		return Msg.wParam;
	} else {
		if (debug)
			OpenConsole();

		if (vid) {
			//int duration = 5, framerate = 20;

			BezierCurve *areaCurve = new BezierCurve();
			areaCurve->addControlPoint2d(0, 10);
			//areaCurve->addControlPoint2d(2.0, 2.0);
			//areaCurve->addControlPoint2d(4.0 / 6.0, 0.001);
			areaCurve->addControlPoint2d(duration / duration, 0.0001);
			//areaCurve->addControlPoint2dWithHandles(0.1, 0.0, duration / duration, 0.000009, 1.3, 0.0);

			BezierCurve *centerCurve = new BezierCurve();
			centerCurve->addControlPoint3d(0, 0, 0);
			//centerCurve->addControlPoint3d(2.0, 0, 0);
			//centerCurve->addControlPoint3d(4.0 / 6.0, 0.043820904830500 - 0.043820904830500 * 4.0 / 6.0, 0.729320429246429 - 0.729320429246429 * 4.0 / 6.0);
			//centerCurve->addControlPoint3d(0.3, 0.043820904830500, 0.729320429246429);
			centerCurve->addControlPoint3d(duration / duration, 0.043820904830500, 0.729320429246429);

			int res[2] = { 1920, 1080 };

			NewtonFraktalGeneration *gen = new NewtonFraktalGeneration();
			NewtonFraktalCLGenerator *clGen = new NewtonFraktalCLGenerator();
			clGen->initCL(0, 0);
			if (clGen->err == CL_SUCCESS) {
				gen->registerGenerator(clGen, gen->GENERATION_MODE_CL);
				gen->setDefaultGenerationMode(gen->GENERATION_MODE_CL);
			}

			Polynom *polynom;
			if (usingDouble())
				polynom = Polynom::getRandomPolynom(MAX_DEGREE);
			else
				polynom = Polynom::getRandomPolynom(MAX_DEGREE_FLOAT);
			delete polynom;
			//polynom = Polynom::readFromString("( -5 + 0 i)*x^ 23 + ( 12 + -1 i)*x^ 22 + ( -16 + -16 i)*x^ 21 + ( -19 + 3 i)*x^ 20 + ( -2 + 19 i)*x^ 19 + ( -12 + 10 i)*x^ 18 + ( -7 + 15 i)*x^ 17 + ( -11 + -12 i)*x^ 16 + ( 2 + 15 i)*x^ 15 + ( -13 + 10 i)*x^ 14 + ( 9 + 13 i)*x^ 13 + ( -7 + 2 i)*x^ 12 + ( 9 + -15 i)*x^ 11 + ( -15 + 8 i)*x^ 10 + ( -13 + 9 i)*x^ 9 + ( 4 + -6 i)*x^ 8 + ( -10 + 2 i)*x^ 7 + ( -13 + -4 i)*x^ 6 + ( -3 + -1 i)*x^ 5 + ( -19 + -20 i)*x^ 4 + ( 1 + 8 i)*x^ 3 + ( 19 + -17 i)*x^ 2 + ( 16 + -19 i)*x + ( 8 + -19 i)");
			polynom->printPolynom();

			gen->generateZoom(areaCurve, centerCurve, framerate, duration, res, polynom, 0.35);
		} else {
			int res[2] = { 2000, 2000 };

			NewtonFraktalGeneration *gen = new NewtonFraktalGeneration();
			NewtonFraktalCLGenerator *clGen = new NewtonFraktalCLGenerator();
			clGen->initCL(0, 0);
			if (clGen->err == CL_SUCCESS) {
				gen->registerGenerator(clGen, gen->GENERATION_MODE_CL);
				gen->setDefaultGenerationMode(gen->GENERATION_MODE_CL);
			}

			NewtonFraktal *fraktal = new NewtonFraktal(res[0], res[1]);
			fraktal->setArea(2, 2);
			fraktal->setCenter(0, 0);
			fraktal->setContrast(0.35);

			Polynom *polynom;
			if (usingDouble())
				polynom = Polynom::getRandomPolynom(MAX_DEGREE);
			else
				polynom = Polynom::getRandomPolynom(MAX_DEGREE_FLOAT);
			fraktal->setPolynom(polynom);
			fraktal->getPolynom()->printPolynom();

			gen->generate(fraktal, gen->getDefaultGenerationMode());
		}

		if (debug) {
			cout << "Please press Return..." << endl;
			getchar();
		}
		return 0;
	}
}