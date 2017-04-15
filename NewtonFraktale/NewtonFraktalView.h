#pragma once
#include "PolycodeView.h"
class NewtonFraktalView :
	public PolycodeViewBase {
public:
	NewtonFraktalView(HINSTANCE hInstance, int nCmdShow, LPCTSTR windowTitle, POINT minSize, bool resizable = false, bool showDebugConsole = false);
	~NewtonFraktalView();

	HWND hwnd;
	static POINT minSize;
};
