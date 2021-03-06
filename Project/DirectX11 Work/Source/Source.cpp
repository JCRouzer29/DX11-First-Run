#include <windows.h>
#include <D3D11.h>
#include <stdio.h>

#pragma comment (lib, "D3D11.lib")

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	// Register windowclass
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.lpszClassName = TEXT("MyClass");
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wc);
	
	// Create window
	HWND hWnd = CreateWindow(
		wc.lpszClassName,
		TEXT("D3D11 Window"),
		WS_OVERLAPPEDWINDOW |
			WS_CLIPSIBLINGS |
			WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	
	// Create device and swapchain
	DXGI_SWAP_CHAIN_DESC scd;
	IDXGISwapChain *pSwapChain;
	ID3D11Device *pDevice;
	ID3D11DeviceContext *pDeviceContext;
	
	ZeroMemory(&scd, sizeof(scd));
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.SampleDesc.Count = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hWnd;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed = TRUE;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	
	HRESULT hResult = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&scd,
		&pSwapChain,
		&pDevice,
		NULL,
		&pDeviceContext
	);
	if(FAILED(hResult)) {
		MessageBox(NULL, TEXT("D3D11CreateDeviceAndSwapChain"), TEXT("D3D11CreateDeviceAndSwapChain"), MB_OK);
		return 0;
	}
	
	// Render target
	ID3D11Texture2D *pBackBuffer;
	ID3D11RenderTargetView *pRTV;
	D3D11_TEXTURE2D_DESC backBufferDesc;
	
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRTV);
	pBackBuffer->GetDesc(&backBufferDesc);
	pBackBuffer->Release();
	
	// Mode switching
	UINT currentMode = 0;
	bool modeChanged = false;
	BOOL currentFullscreen = FALSE;
	pSwapChain->GetFullscreenState(&currentFullscreen, NULL);
	
	// Main loop
	ShowWindow(hWnd, nCmdShow);
	
	bool loop = true;
	while(loop) {
		MSG msg;
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0) {
			if(msg.message == WM_QUIT)
				loop = false;
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {
			// Check for mode change input
			bool changeMode = false;
			if(GetAsyncKeyState(VK_ADD) & 0x8000) {
				if(!modeChanged) {
					++currentMode;
					changeMode = true;
				}
			}
			else if(GetAsyncKeyState(VK_SUBTRACT) & 0x8000) {
				if(!modeChanged) {
					--currentMode;
					changeMode = true;
				}
			}
			else
				modeChanged = false;
			
			// Change mode
			bool changedThisFrame = false;
			
			if(changeMode) {
				IDXGIOutput *pOutput;
				pSwapChain->GetContainingOutput(&pOutput);
				
				UINT numModes = 1024;
				DXGI_MODE_DESC modes[1024];
				pOutput->GetDisplayModeList(scd.BufferDesc.Format, 0, &numModes, modes);
				
				if(currentMode < numModes) {
					DXGI_MODE_DESC mode = modes[currentMode];
					
					TCHAR str[255];
					wsprintf(str, TEXT("Switching to mode: %u / %u, %ux%u@%uHz (%u, %u, %u)\n"),
						currentMode+1,
						numModes,
						mode.Width,
						mode.Height,
						mode.RefreshRate.Numerator / mode.RefreshRate.Denominator,
						mode.Scaling,
						mode.ScanlineOrdering,
						mode.Format
					);
					OutputDebugString(str);
					
					pSwapChain->ResizeTarget(&(modes[currentMode]));
					
					changedThisFrame = true;
				}
				
				modeChanged = true;
			}
			
			// Check fullscreen state
			BOOL newFullscreen;
			pSwapChain->GetFullscreenState(&newFullscreen, NULL);
			
			// Resize if needed
			RECT rect;
			GetClientRect(hWnd, &rect);
			UINT width = static_cast<UINT>(rect.right);
			UINT height = static_cast<UINT>(rect.bottom);
			
			if(width != backBufferDesc.Width || height != backBufferDesc.Height || changedThisFrame || newFullscreen != currentFullscreen) {
				pDeviceContext->ClearState();
				pRTV->Release();
				
				pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
				
				// Recreate render target
				pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
				pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRTV);
				pBackBuffer->GetDesc(&backBufferDesc);
				pBackBuffer->Release();
			}
			
			// Remember fullscreen state
			currentFullscreen = newFullscreen;
			
			// Clear backbuffer
			float color[4] = {1.0f, 0.0f, 1.0f, 1.0f};
			pDeviceContext->ClearRenderTargetView(pRTV, color);
			
			// Present
			pSwapChain->Present(1, 0);
		}
	}
	
	// Release
	pSwapChain->SetFullscreenState(FALSE, NULL);
	
	pDeviceContext->ClearState();
	
	pRTV->Release();
	pDeviceContext->Release();
	
	ID3D11Debug *debugInterface = NULL;
	//hResult = pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debugInterface));
	//if(FAILED(hResult)) {
	//}
	
	pDevice->Release();
	pSwapChain->Release();
	
	if(debugInterface != NULL) {
		debugInterface->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		
		debugInterface->Release();
	}
	
	UnregisterClass(wc.lpszClassName, hInstance);
	
	return 0;
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
		return 0;
	}
	
	return DefWindowProc(hWnd, msg, wParam, lParam);
}