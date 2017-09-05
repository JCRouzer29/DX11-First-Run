#ifndef LAB_2
#define LAB_2

#include "../Include.h"
#include "../Lab.h"
#include "../../CSH/Trivial_VS_2.csh"
#include "../../CSH/Trivial_PS_2.csh"

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500

struct SIMPLE_VERTEX{
	XMFLOAT3	Pos;
	float		Padding1;
	XMFLOAT4	Color;
};

class Lab2 : Lab{
public:
	Lab2();
	~Lab2();

private:
	ID3D11Device			*iDevice;
	ID3D11DeviceContext		*iContext;
	IDXGISwapChain			*iSwapChain;

	ID3D11Texture2D			*iZBuffer;
	ID3D11DepthStencilView	*iDSV;

	ID3D11VertexShader		*iVertexShader;
	ID3D11PixelShader		*iPixelShader;

	ID3D11Buffer			*iVertexBuffer;
	ID3D11Buffer			*iConstBufferGeometry;
	
	ID3D11RenderTargetView	*iRTV;
	D3D11_VIEWPORT			Viewport;
	ID3D11Resource			*pBB;
	ID3D11InputLayout		*iLayout;

//Geometry Data
	SIMPLE_VERTEX Triangle[3];

public:
	bool Setup(HWND window);
	bool Run(XTime xTime);
	bool ShutDown();
};
#endif