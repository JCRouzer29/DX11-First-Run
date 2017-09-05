#ifndef LAB_TEMPLATE
#define LAB_TEMPLATE

#include "../../DirectX11 Work/Source/Include.h"
#include "Lab.h"
#include "Camera.h"

#pragma region Shaders

#pragma endregion
#pragma region Defines

#pragma endregion

class LabNum : public Lab{
public:
	LabNum();
	~LabNum();

	//Class Hierarchy Functions
	bool Setup(HWND window);
	bool Run(XTime xTime);
	bool ShutDown();

private:
	//Variables required for basic functionality
	ID3D11Device			*iDevice;
	ID3D11DeviceContext		*iContext;
	IDXGISwapChain			*iSwapChain;
	
	D3D11_VIEWPORT			Viewport;
	ID3D11RenderTargetView	*iRTV;
	ID3D11DepthStencilView	*iDSV;
	ID3D11Texture2D			*iZBuffer;
	ID3D11Resource			*pBB;

	Camera					*pCamera;
//-----------------------------------------
	//Helper Functions
	HRESULT HelperSetup(HWND window, UINT uiWidth, UINT uiHeight);
	HRESULT CreateConstBuffer(ID3D11Buffer *pConstBuffer, UINT Stride, UINT Count);
	HRESULT CreateConstBuffer(ID3D11Buffer *pConstBuffer, UINT Stride, UINT Count, void* SubData);
	HRESULT MapData(ID3D11Buffer *pBuffer, void* pData, UINT Stride);
//-----------------------------------------

};
#endif