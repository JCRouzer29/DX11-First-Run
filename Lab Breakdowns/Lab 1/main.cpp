// D3D LAB 1a "Line Land".
// Author: L.Norri CD DRX, FullSail University

// Introduction:
// Welcome to the first lab of the Direct3D Graphics Programming class.
// This is the ONLY guided lab in this course! 
// Future labs will demand the habits & foundation you develop right now!  
// It is CRITICAL that you follow each and every step. ESPECIALLY THE READING!!!

// TO BEGIN: Open the word document that acompanies this lab and start from the top.

//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include <iostream>
#include "XTime.h"

using namespace std;

// BEGIN PART 1
// TODO: PART 1 STEP 1a

// TODO: PART 1 STEP 1b

// TODO: PART 2 STEP 6

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500

//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************

class DEMO_APP
{	
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;
	// TODO: PART 1 STEP 2
	
	// TODO: PART 2 STEP 2
	
	// BEGIN PART 5
	// TODO: PART 5 STEP 1
	
	// TODO: PART 2 STEP 4
	
	// BEGIN PART 3
	// TODO: PART 3 STEP 1

	// TODO: PART 3 STEP 2b
	
	// TODO: PART 3 STEP 4a

public:
	// BEGIN PART 2
	// TODO: PART 2 STEP 1
	
	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
};

//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

DEMO_APP::DEMO_APP(HINSTANCE hinst, WNDPROC proc)
{
	// ****************** BEGIN WARNING ***********************// 
	// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY! 
	application = hinst; 
	appWndProc = proc; 

	WNDCLASSEX  wndClass;
    ZeroMemory( &wndClass, sizeof( wndClass ) );
    wndClass.cbSize         = sizeof( WNDCLASSEX );             
    wndClass.lpfnWndProc    = appWndProc;						
    wndClass.lpszClassName  = L"DirectXApplication";            
	wndClass.hInstance      = application;		               
    wndClass.hCursor        = LoadCursor( NULL, IDC_ARROW );    
    wndClass.hbrBackground  = ( HBRUSH )( COLOR_WINDOWFRAME ); 
	wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
    RegisterClassEx( &wndClass );

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(	L"DirectXApplication", L"Lab 1a Line Land",	WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME|WS_MAXIMIZEBOX), 
							CW_USEDEFAULT, CW_USEDEFAULT, window_size.right-window_size.left, window_size.bottom-window_size.top,					
							NULL, NULL,	application, this );												

    ShowWindow( window, SW_SHOW );
	//********************* END WARNING ************************//

	// TODO: PART 1 STEP 3a
	
	// TODO: PART 1 STEP 3b
	
	// TODO: PART 1 STEP 4
	
	// TODO: PART 1 STEP 5
	
	// TODO: PART 2 STEP 3a
	
	// BEGIN PART 4
	// TODO: PART 4 STEP 1

	// TODO: PART 2 STEP 3b
	
    // TODO: PART 2 STEP 3c
	
	// TODO: PART 2 STEP 3d
	
	// TODO: PART 5 STEP 2a
	
	// TODO: PART 5 STEP 2b
	
	// TODO: PART 5 STEP 3
		
	// TODO: PART 2 STEP 5
	// ADD SHADERS TO PROJECT, SET BUILD OPTIONS & COMPILE

	// TODO: PART 2 STEP 7
	
	// TODO: PART 2 STEP 8a
	
	// TODO: PART 2 STEP 8b
    
	// TODO: PART 3 STEP 3

	// TODO: PART 3 STEP 4b

}

//************************************************************
//************ EXECUTION *************************************
//************************************************************

bool DEMO_APP::Run()
{
	// TODO: PART 4 STEP 2	
	
	// TODO: PART 4 STEP 3
	
	// TODO: PART 4 STEP 5
	
	// END PART 4

	// TODO: PART 1 STEP 7a
	
	// TODO: PART 1 STEP 7b
	
	// TODO: PART 1 STEP 7c
	
	// TODO: PART 5 STEP 4
	
	// TODO: PART 5 STEP 5
	
	// TODO: PART 5 STEP 6
	
	// TODO: PART 5 STEP 7
	
	// END PART 5
	
	// TODO: PART 3 STEP 5
	
	// TODO: PART 3 STEP 6
	
	// TODO: PART 2 STEP 9a
	
	// TODO: PART 2 STEP 9b
	
	// TODO: PART 2 STEP 9c
	
	// TODO: PART 2 STEP 9d
	
	// TODO: PART 2 STEP 10
	
	// END PART 2

	// TODO: PART 1 STEP 8
	
	// END OF PART 1
	return true; 
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
	// TODO: PART 1 STEP 6
	
	UnregisterClass( L"DirectXApplication", application ); 
	return true;
}

//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!
	
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,	int nCmdShow );						   
LRESULT CALLBACK WndProc(HWND hWnd,	UINT message, WPARAM wparam, LPARAM lparam );		
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE, LPTSTR, int )
{
	srand(unsigned int(time(0)));
	DEMO_APP myApp(hInstance,(WNDPROC)WndProc);	
    MSG msg; ZeroMemory( &msg, sizeof( msg ) );
    while ( msg.message != WM_QUIT && myApp.Run() )
    {	
	    if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        { 
            TranslateMessage( &msg );
            DispatchMessage( &msg ); 
        }
    }
	myApp.ShutDown(); 
	return 0; 
}
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if(GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
    switch ( message )
    {
        case ( WM_DESTROY ): { PostQuitMessage( 0 ); }
        break;
    }
    return DefWindowProc( hWnd, message, wParam, lParam );
}
//********************* END WARNING ************************//