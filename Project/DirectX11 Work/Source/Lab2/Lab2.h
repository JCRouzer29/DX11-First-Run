#ifndef LAB_2_H
#define LAB_2_H

#include "../../../DX11 Work Base\Source\Lab.h"

#pragma region Shaders
#include "../../CSH/Trivial_VS_2.csh"
#include "../../CSH/Trivial_PS_2.csh"
#pragma endregion
#pragma region Defines
#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500
//Geometry Buffer Information
#define STAR_VERTEX_COUNT	20
#define STAR_INDEX_COUNT	108
#define CUBE_VERTEX_COUNT	8
#define CUBE_INDEX_COUNT	36
//Geometry Random Movement
#define MOV_RNG				12
#define ROT_RNG				180
#define RNDM_PCN			100.0f
#define RANDOM_VELOCITY		((rand() % (MOV_RNG * (int)RNDM_PCN)) / RNDM_PCN) - (MOV_RNG / 2.0f)
#define RANDOM_ROTATION		XMConvertToRadians(((rand() % (ROT_RNG * (int)RNDM_PCN)) / RNDM_PCN) - (ROT_RNG / 2.0f))
//Grid Box Information
#define BOUNDARY			20.0f
#define LN_GRD_BNDRY		BOUNDARY * 2
#define LN_GRD_ROWS			20
#define LN_GRD_COLUMNS		20
#define LN_GRD_STEP_X		LN_GRD_BNDRY / (LN_GRD_COLUMNS - 1)
#define LN_GRD_STEP_Y		LN_GRD_BNDRY / (LN_GRD_ROWS - 1)
#define LN_GRD_VERT_COUNT	(LN_GRD_COLUMNS * 2) + ((LN_GRD_ROWS - 2) * 2)
#define LN_GRD_IND_COUNT	(LN_GRD_COLUMNS * 2) + (LN_GRD_ROWS * 2)
#pragma endregion

struct SIMPLE_VERTEX{
	XMFLOAT4	Color;
	XMFLOAT3	Pos;
	float		Padding1;
};
struct OBJECT{
	XMFLOAT4X4		WorldMatrix;
	ID3D11Buffer	*iConstBuffer;
	ID3D11Buffer	*iVertexBuffer;
	ID3D11Buffer	*iIndexBuffer;
	unsigned short	VertexCount;
	unsigned short	IndexCount;
	XMFLOAT3		Velocity;
	XMFLOAT3		Rotation;
};
struct CAMERA{
	XMFLOAT4X4 WorldMatrix;
};
struct SCENE{
	XMFLOAT4X4 ViewMatrix;
	XMFLOAT4X4 ProjMatrix;
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
	HRESULT CreateConstBuffer(ID3D11Buffer **pConstBuffer, UINT Stride, UINT Count);
	HRESULT CreateConstBuffer(ID3D11Buffer **pConstBuffer, UINT Stride, UINT Count, void* SubData);
	HRESULT MapData(ID3D11Buffer *pBuffer, void* pData, UINT Stride);

	void UpdateCamera(XTime xTime);
	void UpdateGeometry(XTime xTime);
	void CheckGeometry();
//-----------------------------------------
	
	ID3D11DepthStencilView	*iDSV;
	ID3D11Texture2D			*iZBuffer;

//Wireframe
	bool					wireframe;
	ID3D11RasterizerState	*iRasterState;
	ID3D11RasterizerState	*iRasterStateWire;

	ID3D11VertexShader		*iVertexShader;
	ID3D11PixelShader		*iPixelShader;

	ID3D11Buffer			*iConstBufferCamera;
	ID3D11Buffer			*iConstBufferScene;

	ID3D11InputLayout		*iLayout;

//Geometry Data
	CAMERA Camera;
	SCENE Scene;

	OBJECT StarList[3];
	OBJECT CubeList[3];
	OBJECT GridPanels[6];
};
#endif