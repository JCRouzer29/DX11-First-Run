#ifndef LAB_TEMPLATE
#define LAB_TEMPLATE

#include "../Include.h"
#include "../Lab.h"

#pragma region Shaders
#include "../../CSH/Trivial_VS_2.csh"
#include "../../CSH/Trivial_PS_2.csh"
#pragma endregion
#pragma region Defines
#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500
#pragma endregion

struct SIMPLE_VERTEX{
	XMFLOAT3	Pos;
	float		Padding1;
	XMFLOAT4	Color;
};

class Lab2 : public Lab{
public:
	Lab2();
	~Lab2();

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
	ID3D11Resource			*pBB;
//-----------------------------------------
	//Helper Functions
	HRESULT HelperSetup(HWND window, UINT uiWidth, UINT uiHeight);
	HRESULT CreateConstBuffer(ID3D11Buffer *pConstBuffer, UINT Stride, UINT Count, void* SubData);
	HRESULT MapData(ID3D11Buffer *pBuffer, void* pData, UINT Stride);
//-----------------------------------------
	
	ID3D11DepthStencilView	*iDSV;
	ID3D11Texture2D			*iZBuffer;

	ID3D11VertexShader		*iVertexShader;
	ID3D11PixelShader		*iPixelShader;

	ID3D11Buffer			*iVertexBuffer;
	ID3D11Buffer			*iConstBufferGeometry;

	ID3D11InputLayout		*iLayout;

//Geometry Data
	SIMPLE_VERTEX Triangle[3];
};
#endif