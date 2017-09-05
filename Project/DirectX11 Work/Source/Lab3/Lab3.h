#ifndef LAB_3_H
#define LAB_3_H

#include "../../../DX11 Work Base\Source\Lab.h"
#include "../../../DX11 Work Base\Source\Camera.h"
#include "../../../DX11 Work Base\Source\safe_thread.h"
#include "Object.h"
#include <vector>

#pragma region Shaders
#include "../../CSH/VS_Basic.csh"
#include "../../CSH/VS_Skybox.csh"
#include "../../CSH/PS_Texture.csh"
#include "../../CSH/PS_Skybox.csh"
#pragma endregion
#pragma region Defines
#define BACKBUFFER_WIDTH	500
#define BACKBUFFER_HEIGHT	500
//Geometry Buffer Information
#define CUBE_VERTEX_COUNT	24
#define CUBE_INDEX_COUNT	36
#define CUBE_ROT_RATE_X		XMConvertToRadians(60.0f)
#define CUBE_ROT_RATE_Y		XMConvertToRadians(30.0f)
#define CUBE_ROT_RATE_Z		XMConvertToRadians(60.0f)
#pragma endregion

struct SIMPLE_VERTEX{
	XMFLOAT4	Color;
	XMFLOAT3	Pos;
	float		Padding1;
	XMFLOAT2	TexCoord;
//	XMFLOAT2	Padding2;
};

struct SIMPLE_TEXTURE{
	ID3D11Resource				*iResource;
	ID3D11ShaderResourceView	*iSRV;
};

class Lab3 : public Lab{
public:
	Lab3();
	~Lab3();

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

	ID3D11VertexShader		*iVertexShader;
	ID3D11PixelShader		*iPixelShader;

	ID3D11InputLayout		*iLayout;

	Camera					*pCamera;
//-----------------------------------------
	//Helper Functions
	HRESULT HelperSetup(HWND window, UINT uiWidth, UINT uiHeight);
	HRESULT CreateConstBuffer(ID3D11Buffer **pConstBuffer, UINT Stride, UINT Count);
	HRESULT CreateConstBuffer(ID3D11Buffer **pConstBuffer, UINT Stride, UINT Count, void* SubData);
	HRESULT MapData(ID3D11Buffer *pBuffer, void* pData, UINT Stride);
	HRESULT LoadTexture(const wchar_t *szFileName, ID3D11Resource **rTexture, ID3D11ShaderResourceView **SRV);

	void RenderTexturedCube(ID3D11DeviceContext *pContext);
	void LoadAllTextures();
	void SwapCubeTexture();
	void ResetSkyboxPosition();
//-----------------------------------------
	ID3D11DeviceContext		*iDeferredContext;
	ID3D11CommandList		*iCommandListRenderCube;

	Object						Cube;
	ID3D11SamplerState			*iSamplerWrap;
	std::vector<std::string>	szTextureFileNames;
	std::vector<std::string>	szSkyboxFileNames;
	std::vector<SIMPLE_TEXTURE>	iTextures;
	unsigned int				currentTexture;
	double						textureTimer;
	safe_thread					LoadingThread;

	//Skybox
	Object					Skybox;
	SIMPLE_TEXTURE			iSkyboxTexture;
	ID3D11VertexShader		*iVS_Skybox;
	ID3D11PixelShader		*iPS_Skybox;

	//Rasterizer states
	bool					wireframe;
	ID3D11RasterizerState	*iRasterState;
	ID3D11RasterizerState	*iRasterStateWire;
	ID3D11RasterizerState	*iRasterStateCulling;
};
#endif