//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include <vld.h>
#include <time.h>
#include "Include.h"
#include "IncludeLabs.h"

//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************

class Project{
	HINSTANCE				application;
	WNDPROC					appWndProc;
	HWND					window;

	//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	LAB						nRun;

	XTime					xTime;

public:
	//static LRESULT CALLBACK WndProcF(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	Project(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
};

//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

Project::Project(HINSTANCE AppHI, WNDPROC WndProc){
	WNDCLASSEX wndClass;
    ZeroMemory(&wndClass, sizeof(wndClass));
    wndClass.cbSize         = sizeof(WNDCLASSEX);
    wndClass.lpfnWndProc    = WndProc;
    wndClass.lpszClassName  = L"DirectXApplication";
	wndClass.hInstance      = AppHI;
    wndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground  = (HBRUSH)(COLOR_WINDOWFRAME);
//	wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
    RegisterClassEx(&wndClass);

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(	L"DirectXApplication", L"Lab Solution",	WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME|WS_MAXIMIZEBOX),
							CW_USEDEFAULT, CW_USEDEFAULT, window_size.right-window_size.left, window_size.bottom-window_size.top,
							NULL, NULL,	AppHI, this);

	srand((unsigned int)time(NULL));

	//SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)this);

    ShowWindow(window, SW_SHOW);

	nRun.Setup(window);
}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool Project::Run(){
	xTime.Signal();

	nRun.Run(xTime);

	return true; 
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool Project::ShutDown(){
	nRun.ShutDown();

	UnregisterClass( L"DirectXApplication", application ); 
	return true;
}

//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    if(GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
    switch(message){
        case (WM_DESTROY):{
			PostQuitMessage(0);
			break;
		}
		case (WM_SIZE):{

			break;
		}
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int){
	//Project myApp(hInstance,(WNDPROC)(Project::WndProcF));
	Project myApp(hInstance,(WNDPROC)(WndProc));
    MSG msg; ZeroMemory(&msg, sizeof(msg));
    while(msg.message != WM_QUIT && myApp.Run()){
	    if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
	myApp.ShutDown();
	return 0;
}
/*LRESULT CALLBACK Project::WndProcF(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	//Supposed to work with static cast but its giving me issues, reinterpret cast fails

	LONG_PTR thisLong = GetWindowLongPtr(hWnd, GWLP_USERDATA);

	Project *thisProj = (Project*)(thisLong);
	//Project *thisProj = reinterpret_cast<Project*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	if(thisProj)
		return thisProj->WndProc(hWnd, message, wParam, lParam);
	return DefWindowProc(hWnd, message, wParam, lParam);
}//*/