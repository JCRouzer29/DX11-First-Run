#ifndef LAB_1_H
#define LAB_1_H

#include "../../../DX11 Work Base\Source\Lab.h"
#include "../../CSH/Trivial_VS.csh"
#include "../../CSH/Trivial_PS.csh"

#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500
#define CIRCLE_VERT_COUNT	361

struct SEND_TO_VRAM{
	XMFLOAT4	Color;
	XMFLOAT2	Offset;
	XMFLOAT2	Padding;
};
struct SIMPLE_VERTEX{
	XMFLOAT2	pos;
};

class Lab1 : Lab{
public:
	Lab1();
	~Lab1();
private:

	ID3D11Device			*iDevice;
	ID3D11DeviceContext		*iContext;
	IDXGISwapChain			*iSwapChain;

	ID3D11RenderTargetView	*iRTV;
	D3D11_VIEWPORT			Viewport;
	ID3D11Resource			*pBB;
	
	ID3D11Buffer			*iCircleBuffer;
	int						VertexCount;
	
	ID3D11Buffer			*iGridVertBuffer;
	ID3D11Buffer			*iGridConstBuffer;
	int						GridVertexCount;
	
	ID3D11VertexShader		*iVertexShader;
	ID3D11PixelShader		*iPixelShader;

	ID3D11InputLayout		*iLayout;
	
	ID3D11Buffer			*iConstBuffer;

	SEND_TO_VRAM toShader;
	SEND_TO_VRAM GridToShader;

	float HMove, VMove;
public:

	bool Setup(HWND window);
	bool Run(XTime xTime);
	bool ShutDown();
};
#endif