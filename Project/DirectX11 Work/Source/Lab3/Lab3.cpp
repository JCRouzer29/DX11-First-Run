//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include "Lab3.h"
#include "../../../DX11 Work Base\Source\DDSTextureLoader.h"

Lab3::Lab3(){
	iDevice		= nullptr;
	iContext	= nullptr;
	iSwapChain	= nullptr;
	iRTV		= nullptr;
	iDSV		= nullptr;
	iZBuffer	= nullptr;
	pBB			= nullptr;

	pCamera		= new Camera;
//---------------------------
	Cube.SetWorldMatrixXMM(XMMatrixIdentity());
	Cube.SetVelocityXMF3(XMFLOAT3(0.0f, 0.0f, 0.0f));
	Cube.SetRotationRateXMF3(XMFLOAT3(CUBE_ROT_RATE_X, CUBE_ROT_RATE_Y, CUBE_ROT_RATE_Z));
	Skybox.SetWorldMatrixXMM(XMMatrixIdentity());
	Skybox.SetVelocityXMF3(XMFLOAT3(0.0f, 0.0f, 0.0f));
	Skybox.SetRotationRateXMF3(XMFLOAT3(0.0f, 0.0f, 0.0f));

	iSamplerWrap = nullptr;
	iDeferredContext = nullptr;
	iCommandListRenderCube = nullptr;
	currentTexture = 0;
	textureTimer = 0.0;

	iVS_Skybox = nullptr;
	iPS_Skybox = nullptr;

	iSkyboxTexture.iResource = nullptr;
	iSkyboxTexture.iSRV = nullptr;

	wireframe = false;
	iRasterState = nullptr;
	iRasterStateWire = nullptr;
	iRasterStateCulling = nullptr;
}
Lab3::~Lab3(){ }

#pragma region HELPER FUNCTIONS
HRESULT Lab3::HelperSetup(HWND window, UINT uiWidth, UINT uiHeight){
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.BufferDesc.Width = uiWidth;
	SwapChainDesc.BufferDesc.Height = uiHeight;
	SwapChainDesc.Windowed = true;
	SwapChainDesc.OutputWindow = window;

	D3D_FEATURE_LEVEL pFeatureLevel[] = {
		D3D_FEATURE_LEVEL_10_0,
	};

	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 
#if _DEBUG
		D3D11_CREATE_DEVICE_DEBUG
#else
		NULL
#endif
		, pFeatureLevel, 1, D3D11_SDK_VERSION, &SwapChainDesc, &iSwapChain, &iDevice, 0, &iContext);

	assert(S_OK == hr);

	iSwapChain->GetBuffer(0, __uuidof(pBB), reinterpret_cast<void**>(&pBB));
	hr = iDevice->CreateRenderTargetView(pBB, NULL, &iRTV);
	
	assert(S_OK == hr);

	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	Viewport.Height = (float)SwapChainDesc.BufferDesc.Height;
	Viewport.Width = (float)SwapChainDesc.BufferDesc.Width;
	Viewport.TopLeftX = Viewport.TopLeftY = 0;

	D3D11_TEXTURE2D_DESC ZBufferDesc;
	ZeroMemory(&ZBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
	ZBufferDesc.Width = SwapChainDesc.BufferDesc.Width;
	ZBufferDesc.Height = SwapChainDesc.BufferDesc.Height;
	ZBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ZBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	ZBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ZBufferDesc.CPUAccessFlags = NULL;
	ZBufferDesc.ArraySize = 1;
	ZBufferDesc.MipLevels = 1;
	ZBufferDesc.SampleDesc.Count = 1;

	hr = iDevice->CreateTexture2D(&ZBufferDesc, nullptr, &iZBuffer);
	assert(S_OK == hr);

	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	ZeroMemory(&DSVDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	DSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSVDesc.Texture2D.MipSlice = 0;

	hr = iDevice->CreateDepthStencilView(iZBuffer, &DSVDesc, &iDSV);
	assert(S_OK == hr);
	
	ID3D11Buffer *CamConstBuffer = nullptr;
	CreateConstBuffer(&CamConstBuffer, sizeof(XMFLOAT4X4) * 4, 1);
	pCamera->SetConstBuffer(CamConstBuffer);

	return S_OK;
}

HRESULT Lab3::CreateConstBuffer(ID3D11Buffer **pConstBuffer, UINT Stride, UINT Count){
	D3D11_BUFFER_DESC iConstBufferDesc;
	ZeroMemory(&iConstBufferDesc, sizeof(D3D11_BUFFER_DESC));
	iConstBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	iConstBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	iConstBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	iConstBufferDesc.ByteWidth = Stride * Count;
	iConstBufferDesc.StructureByteStride = Stride;

	HRESULT hr = iDevice->CreateBuffer(&iConstBufferDesc, nullptr, pConstBuffer);
	assert(S_OK == hr);

	return S_OK;
}
HRESULT Lab3::CreateConstBuffer(ID3D11Buffer **pConstBuffer, UINT Stride, UINT Count, void* SubData){
	D3D11_BUFFER_DESC iConstBufferDesc;
	ZeroMemory(&iConstBufferDesc, sizeof(D3D11_BUFFER_DESC));
	iConstBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	iConstBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	iConstBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	iConstBufferDesc.ByteWidth = Stride * Count;
	iConstBufferDesc.StructureByteStride = Stride;

	D3D11_SUBRESOURCE_DATA iBufferData;
	ZeroMemory(&iBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	iBufferData.pSysMem = SubData;

	HRESULT hr = iDevice->CreateBuffer(&iConstBufferDesc, &iBufferData, pConstBuffer);
	assert(S_OK == hr);

	return S_OK;
}

HRESULT Lab3::MapData(ID3D11Buffer *pBuffer, void* pData, UINT Stride){
	D3D11_MAPPED_SUBRESOURCE MapResource;
	ZeroMemory(&MapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	MapResource.pData = pData;
	MapResource.DepthPitch = Stride;
	MapResource.RowPitch = Stride;

//	Possible bug to watch for with using Stride here, might not work with arrays
//		Other implementations were "memcpy(MapResource.pData, &pData, sizeof(pData))"
//		Where pData is not a void pointer but the raw data
	iContext->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapResource);
	memcpy(MapResource.pData, pData, Stride);
	iContext->Unmap(pBuffer, 0);

	return S_OK;
}

HRESULT Lab3::LoadTexture(const wchar_t *szFileName, ID3D11Resource **rTexture, ID3D11ShaderResourceView **SRV){
	HRESULT hr = CreateDDSTextureFromFile(iDevice, szFileName, rTexture, SRV);
	assert(S_OK == hr);

	return S_OK;
}

#if _DEBUG
#include <strsafe.h>
void ErrorExit(LPTSTR lpszFunction){ 
    // Retrieve the system error message for the last-error code

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError();

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process
    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
        (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf,
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"),
        lpszFunction, dw, lpMsgBuf);
    MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
	if(0 != dw)
	    ExitProcess(dw);
}
#endif
#pragma endregion

void Lab3::RenderTexturedCube(ID3D11DeviceContext *pContext){
	pContext->OMSetRenderTargets(1, &iRTV, iDSV);
	pContext->RSSetViewports(1, &Viewport);
	
	UINT Strides = sizeof(SIMPLE_VERTEX);
	UINT Offset = 0;

	pContext->VSSetConstantBuffers(0, 1, pCamera->GetConstBufferAdd());
	pContext->IASetInputLayout(iLayout);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

//Skybox
	pContext->RSSetState(iRasterStateCulling);
	pContext->VSSetShader(iVS_Skybox, nullptr, NULL);
	pContext->PSSetShader(iPS_Skybox, nullptr, NULL);

	ID3D11Buffer *IndBuff = Skybox.GetIndexBuffer();

	pContext->IASetVertexBuffers(0, 1, Skybox.GetVertexBufferAdd(), &Strides, &Offset);
	pContext->IASetIndexBuffer(Skybox.GetIndexBuffer(), DXGI_FORMAT_R16_UINT, NULL);
	pContext->VSSetConstantBuffers(1, 1, Skybox.GetConstBufferAdd());
	//Set Texture(s)
	pContext->PSSetShaderResources(0, 1, &iSkyboxTexture.iSRV);
	pContext->PSSetSamplers(0, 1, &iSamplerWrap);		//Same should work...Right?
//----------------------------------------------------------------------------
	pContext->DrawIndexed(Skybox.GetIndexCount(), 0, NULL);//*/

//Textured Box
	pContext->RSSetState(iRasterState);
	
	pContext->VSSetShader(iVertexShader, nullptr, NULL);
	pContext->PSSetShader(iPixelShader, nullptr, NULL);
	
	pContext->IASetVertexBuffers(0, 1, Cube.GetVertexBufferAdd(), &Strides, &Offset);
	pContext->IASetIndexBuffer(Cube.GetIndexBuffer(), DXGI_FORMAT_R16_UINT, NULL);
	pContext->VSSetConstantBuffers(1, 1, Cube.GetConstBufferAdd());
	pContext->PSSetShaderResources(0, 1, &iTextures[currentTexture].iSRV);
	pContext->PSSetSamplers(0, 1, &iSamplerWrap);
	pContext->DrawIndexed(Cube.GetIndexCount(), 0, NULL);

	pContext->FinishCommandList(NULL, &iCommandListRenderCube);
}

//Function uses Windows File API to get list of names of all files in folder
vector<string> AllDDSFilesInFolder(string folder){
	vector<string> names;
	wchar_t search_path[200];
	string path = folder;
	path += std::string("*.*");
	mbstowcs(&search_path[0], path.c_str(), 200);
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(search_path, &fd);
//	ErrorExit(TEXT("FindFirstFileW"));
	if(hFind != INVALID_HANDLE_VALUE){
		do{
			// read all (real) files in current folder, delete '!' read other 2 default folder . and ..
			if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ){
				string fileName = folder;

				//convert from wide char to narrow char array
				char ch[260];
				char DefChar = ' ';
				WideCharToMultiByte(CP_ACP, 0, fd.cFileName, -1, ch, 260, &DefChar, NULL);
				
				//A std:string  using the char* constructor.
				fileName += std::string(ch);
				names.push_back(fileName);
			}
		} while(::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}

void Lab3::LoadAllTextures(){
	szTextureFileNames = AllDDSFilesInFolder("Textures\\");

	std::wstring wString;
	for(size_t iter = 0; iter < szTextureFileNames.size(); iter++){
		SIMPLE_TEXTURE nextTexture;
		wString = std::wstring(szTextureFileNames[iter].begin(), szTextureFileNames[iter].end());
		LoadTexture(wString.c_str(), &nextTexture.iResource, &nextTexture.iSRV);
		iTextures.push_back(nextTexture);
	}
	SwapCubeTexture();

	szSkyboxFileNames = AllDDSFilesInFolder("Textures\\Skybox\\");
	if(szSkyboxFileNames.size() == 0){
		return;
	}
	wString = std::wstring(szSkyboxFileNames[0].begin(), szSkyboxFileNames[0].end());
	LoadTexture(wString.c_str(), &iSkyboxTexture.iResource, &iSkyboxTexture.iSRV);
}
void Lab3::SwapCubeTexture(){
//	currentTexture = rand() % iTextures.size();
	currentTexture++;
	if(iTextures.size() <= currentTexture){
		currentTexture = 0;
	}
	iCommandListRenderCube->Release();
	RenderTexturedCube(iDeferredContext);
}
void Lab3::ResetSkyboxPosition(){
	XMFLOAT4X4 mat;
	XMStoreFloat4x4(&mat, XMMatrixInverse(NULL, XMLoadFloat4x4(&pCamera->GetMatrixPack()->ViewMatrix)));
	Skybox.SetPosition(XMFLOAT3(mat._41, mat._42, mat._43));
}

bool Lab3::Setup(HWND window){
	HRESULT hr = HelperSetup(window, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT);
	assert(S_OK == hr);
//---------------------------
	hr = iDevice->CreateDeferredContext(NULL, &iDeferredContext);

	hr = iDevice->CreateVertexShader(&VS_Basic, sizeof(VS_Basic), nullptr, &iVertexShader);
	hr = iDevice->CreateVertexShader(&VS_Skybox, sizeof(VS_Skybox), nullptr, &iVS_Skybox);
	hr = iDevice->CreatePixelShader(&PS_Texture, sizeof(PS_Texture), nullptr, &iPixelShader);
	hr = iDevice->CreatePixelShader(&PS_Skybox, sizeof(PS_Skybox), nullptr, &iPS_Skybox);
	
	D3D11_INPUT_ELEMENT_DESC SimpleLayout[] = {
		{"COLOR",	 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"PADDING_", 0, DXGI_FORMAT_R32_FLOAT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{"PADDING2", 0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	hr = iDevice->CreateInputLayout(SimpleLayout, 4, &VS_Basic, sizeof(VS_Basic), &iLayout);
	assert(S_OK == hr);
	
	hr = CreateConstBuffer(Cube.GetConstBufferAddress(), sizeof(XMFLOAT4X4), 1);
	assert(S_OK == hr);
	hr = CreateConstBuffer(Skybox.GetConstBufferAddress(), sizeof(XMFLOAT4X4), 1);
	assert(S_OK == hr);

#pragma region TEXTURES
	LoadingThread = std::thread(&Lab3::LoadAllTextures, this);

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	//D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MinLOD = NULL;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	hr = iDevice->CreateSamplerState(&samplerDesc, &iSamplerWrap);
	assert(S_OK == hr);
#pragma endregion
#pragma region CUBE DATA
	SIMPLE_VERTEX CubeVerts[] = {
//						COLOR				POS
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3(-1,  1, -1), 0, XMFLOAT2(0, 0) }, //0 Front
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3( 1,  1, -1), 0, XMFLOAT2(1, 0) }, //1
		{ XMFLOAT4(0, 1, 1, 1), XMFLOAT3( 1, -1, -1), 0, XMFLOAT2(1, 1) }, //2
		{ XMFLOAT4(0, 1, 1, 1), XMFLOAT3(-1, -1, -1), 0, XMFLOAT2(0, 1) }, //3

		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3( 1,  1,  1), 0, XMFLOAT2(0, 0) }, //4 Back
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3(-1,  1,  1), 0, XMFLOAT2(1, 0) }, //5
		{ XMFLOAT4(0, 1, 1, 1), XMFLOAT3(-1, -1,  1), 0, XMFLOAT2(1, 1) }, //6
		{ XMFLOAT4(0, 1, 1, 1), XMFLOAT3( 1, -1,  1), 0, XMFLOAT2(0, 1) }, //7
		
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3( 1,  1, -1), 0, XMFLOAT2(0, 0) }, //8 Left
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3( 1,  1,  1), 0, XMFLOAT2(1, 0) }, //9
		{ XMFLOAT4(0, 1, 1, 1), XMFLOAT3( 1, -1,  1), 0, XMFLOAT2(1, 1) }, //10
		{ XMFLOAT4(0, 1, 1, 1), XMFLOAT3( 1, -1, -1), 0, XMFLOAT2(0, 1) }, //11

		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3(-1,  1,  1), 0, XMFLOAT2(0, 0) }, //12 Right
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3(-1,  1, -1), 0, XMFLOAT2(1, 0) }, //13
		{ XMFLOAT4(0, 1, 1, 1), XMFLOAT3(-1, -1, -1), 0, XMFLOAT2(1, 1) }, //14
		{ XMFLOAT4(0, 1, 1, 1), XMFLOAT3(-1, -1,  1), 0, XMFLOAT2(0, 1) }, //15
		
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3( 1,  1, -1), 0, XMFLOAT2(0, 0) }, //16 Top
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3(-1,  1, -1), 0, XMFLOAT2(1, 0) }, //17
		{ XMFLOAT4(0, 1, 1, 1), XMFLOAT3(-1,  1,  1), 0, XMFLOAT2(1, 1) }, //18
		{ XMFLOAT4(0, 1, 1, 1), XMFLOAT3( 1,  1,  1), 0, XMFLOAT2(0, 1) }, //19

		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3(-1, -1,  1), 0, XMFLOAT2(0, 0) }, //20 Bottom
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3( 1, -1,  1), 0, XMFLOAT2(1, 0) }, //21
		{ XMFLOAT4(0, 1, 1, 1), XMFLOAT3( 1, -1, -1), 0, XMFLOAT2(1, 1) }, //22
		{ XMFLOAT4(0, 1, 1, 1), XMFLOAT3(-1, -1, -1), 0, XMFLOAT2(0, 1) }, //23	//*/
	};
	for(int iter = 0; iter < CUBE_VERTEX_COUNT; iter++){
		CubeVerts[iter].Pos.x *= 10.0f;
		CubeVerts[iter].Pos.y *= 10.0f;
		CubeVerts[iter].Pos.z *= 10.0f;
	}

/*	unsigned short CubeIndex[] = {
	//	Face 1
		0, 1, 2,
		2, 3, 0,
	//	Face 2
		4, 5, 6,
		6, 7, 4,
	//	Face 3
		1, 4, 7,
		7, 2, 1,
	//	Face 4
		3, 2, 7,
		7, 6, 3,
	//	Face 5
		5, 0, 3,
		3, 6, 5,
	//	Face 6
		5, 4, 1,
		1, 0, 5,
	};//*/
	unsigned short CubeIndex[] = {
		0, 1, 2,	//Front
		2, 3, 0,
		4, 5, 6,	//Back
		6, 7, 4,
		8, 9, 10,	//Left
		10, 11, 8,
		12, 13, 14,	//Right
		14, 15, 12,
		16, 17, 18,	//Top
		18, 19, 16,
		22, 21, 20,	//Bottom
		20, 23, 22,
	};
/*	Not Needed	
	unsigned short ReverseCubeIndex[] = {
		2, 1, 0,	//Front
		0, 3, 2,
		6, 5, 4,	//Back
		4, 7, 6,
		10, 9, 8,	//Left
		8, 11, 10,
		14, 13, 12,	//Right
		12, 15, 14,
		18, 17, 16,	//Top
		16, 19, 18,
		20, 21, 22,	//Bottom
		22, 23, 20,
	};//*/
#pragma endregion
	
	D3D11_BUFFER_DESC BufferDesc;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
	BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

#pragma region VERTEX BUFFERS
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.StructureByteStride = sizeof(SIMPLE_VERTEX);

	//Cube
	Cube.SetVertexCount(CUBE_VERTEX_COUNT);
	Cube.SetIndexCount(CUBE_INDEX_COUNT);
	BufferDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * CUBE_VERTEX_COUNT;
	InitData.pSysMem = &CubeVerts;
	hr = iDevice->CreateBuffer(&BufferDesc, &InitData, Cube.GetVertexBufferAddress());
	assert(S_OK == hr);

	//Skybox
	Skybox.SetVertexCount(CUBE_VERTEX_COUNT);
	Skybox.SetIndexCount(CUBE_INDEX_COUNT);
//Scale the cube
	for(int iter = 0; iter < CUBE_VERTEX_COUNT; iter++){
		CubeVerts[iter].Pos.x *= 10.0f;
		CubeVerts[iter].Pos.y *= 10.0f;
		CubeVerts[iter].Pos.z *= 10.0f;
	}
	hr = iDevice->CreateBuffer(&BufferDesc, &InitData, Skybox.GetVertexBufferAddress());
	assert(S_OK == hr);

#pragma endregion
#pragma region INDEX BUFFERS
	BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	BufferDesc.StructureByteStride = sizeof(unsigned short);

	//Cube
	BufferDesc.ByteWidth = sizeof(unsigned short) * CUBE_INDEX_COUNT;
	InitData.pSysMem = &CubeIndex;
	hr = iDevice->CreateBuffer(&BufferDesc, &InitData, Cube.GetIndexBufferAddress());
	assert(S_OK == hr);

	//Skybox
	ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
	InitData.pSysMem = &CubeIndex;
	hr = iDevice->CreateBuffer(&BufferDesc, &InitData, Skybox.GetIndexBufferAddress());
	assert(S_OK == hr);

#pragma endregion

#pragma region RASTERIZER STATES
	D3D11_RASTERIZER_DESC RasterDesc;
	ZeroMemory(&RasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	RasterDesc.FillMode = D3D11_FILL_SOLID;
	RasterDesc.CullMode = D3D11_CULL_BACK;
	RasterDesc.DepthClipEnable = true;

	//Solid State
	hr = iDevice->CreateRasterizerState(&RasterDesc, &iRasterState);
	assert(S_OK == hr);

	//Reverse Culling State
	//RasterDesc.CullMode = D3D11_CULL_NONE;
	RasterDesc.CullMode = D3D11_CULL_FRONT;
	
	hr = iDevice->CreateRasterizerState(&RasterDesc, &iRasterStateCulling);
	assert(S_OK == hr);
	
	//Wireframe State
	RasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	RasterDesc.CullMode = D3D11_CULL_NONE;

	hr = iDevice->CreateRasterizerState(&RasterDesc, &iRasterStateWire);
	assert(S_OK == hr);
#pragma endregion
	
	safe_thread CubeThread = std::thread(&Lab3::RenderTexturedCube, this, iDeferredContext);
	
	XMStoreFloat4x4(&pCamera->GetMatrixPack()->WorldMatrix, XMMatrixLookAtLH(XMLoadFloat3(&XMFLOAT3(22.0f, 20.0f, 22.0f)), XMLoadFloat3(&XMFLOAT3(0, 0, 0)), XMLoadFloat3(&XMFLOAT3(0, 1.0f, 0))));
	XMStoreFloat4x4(&pCamera->GetMatrixPack()->ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&pCamera->GetMatrixPack()->ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(75.0f), 1, 1.0f, 1000.0f));

	if(LoadingThread.joinable())	LoadingThread.join();
	if(CubeThread.joinable())		CubeThread.join();

	return true;
}
bool Lab3::Run(XTime xTime){
	iContext->OMSetRenderTargets(1, &iRTV, iDSV);
	iContext->RSSetViewports(1, &Viewport);
	float CLEAR_COLOR[] = {255, 0, 0, 1};
	iContext->ClearRenderTargetView(iRTV, CLEAR_COLOR);
	iContext->ClearDepthStencilView(iDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	pCamera->Update(xTime);
	MapData(pCamera->GetConstBuffer(), pCamera->GetMatrixPack(), sizeof(MatPack));
//	ResetSkyboxPosition();
//---------------------------
	textureTimer += xTime.Delta();
	if(textureTimer >= 1.5){
		//send out new thread for remaking command list
		LoadingThread = std::thread(&Lab3::SwapCubeTexture, this);
		textureTimer = 0.0;
	}

	Cube.Update(xTime);
	Cube.MapConstBuffer(iContext);

	//catch the thread about remaking command list
	if(LoadingThread.joinable()) LoadingThread.join();
	iContext->ExecuteCommandList(iCommandListRenderCube, NULL);

//---------------------------
	iSwapChain->Present(0, 0);

	return true;
}
bool Lab3::ShutDown(){
	iContext->ClearState();
	iRTV->Release();
	iDSV->Release();
	iZBuffer->Release();
	pBB->Release();

	pCamera->ReleaseConstBuffer();
	delete pCamera;
//---------------------------
	Cube.Release();
	Skybox.Release();

	iSamplerWrap->Release();
	for(size_t iter = 0; iter < iTextures.size(); iter++){
		iTextures[iter].iResource->Release();
		iTextures[iter].iSRV->Release();
	}
	iSkyboxTexture.iResource->Release();
	iSkyboxTexture.iSRV->Release();

	iVertexShader->Release();
	iPixelShader->Release();
	iVS_Skybox->Release();
	iPS_Skybox->Release();

	iLayout->Release();

	iDeferredContext->Release();
	if(iCommandListRenderCube){
		iCommandListRenderCube->Release();
	}

	iRasterState->Release();
	iRasterStateWire->Release();
	iRasterStateCulling->Release();
//---------------------------
	iSwapChain->Release();
	iContext->Release();
	iDevice->Release();

	return true;
}


/*
	Current Issue
	still no skybox
	not making it past vertex shader


	-Not
	Culling
	Position
	PS or Texture
	Vertices
	Indices
	Size
*/