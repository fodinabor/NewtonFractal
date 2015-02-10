#include <PolycodeView.h>
#include "windows.h"
#include "NewtonFraktalApp.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	PolycodeView *view = new PolycodeView(hInstance, nCmdShow, L"NewtonFraktale", false, true);
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
}