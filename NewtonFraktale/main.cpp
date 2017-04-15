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

const InstructionSet::InstructionSet_Internal InstructionSet::CPU_Rep;
extern void OpenConsole();

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	bool debug = false, createWindow = true;
	int nArgs;
	LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	for (int i = 1; i < nArgs; i++) {
		String arg = String(szArglist[i]);
		if (arg == "-d" || arg == "/d")
			debug = true;
		else if (arg == "-q" || arg == "/q")
			createWindow = false;
	}

	srand(time(NULL));

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
		Logger::getInstance()->setLogToFile(true);
		//OpenConsole();

		int duration = 5, framerate = 25;
		BezierCurve *areaCurve = new BezierCurve();
		areaCurve->addControlPoint2d(0, 10);
		areaCurve->addControlPoint2d(2.0, 2.0);
		areaCurve->addControlPoint2d(5.0, 0.8);

		BezierCurve *centerCurve = new BezierCurve();
		centerCurve->addControlPoint3d(0, 0, 0);
		centerCurve->addControlPoint3d(2.0, 0, 0);
		centerCurve->addControlPoint3d(5.0, 0.48, 0.0);

		__declspec(align(MEM_ALIGN)) int res[2] = { 1000, 1000 };

		NewtonFraktalGeneration *gen = new NewtonFraktalGeneration();
		NewtonFraktalCLGenerator *clGen = new NewtonFraktalCLGenerator();
		clGen->initCL(0, 0);
		gen->registerGenerator(clGen, gen->GENERATION_MODE_CL);
		gen->setDefaultGenerationMode(gen->GENERATION_MODE_CL);

		gen->generateZoom(areaCurve, centerCurve, framerate, duration, res, Polynom::getRandomPolynom(23), 0.35);

		return 0;
	}
}