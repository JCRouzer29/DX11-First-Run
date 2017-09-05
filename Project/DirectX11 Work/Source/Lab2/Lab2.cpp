//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include "Lab2.h"

Lab2::Lab2(){
	iDevice		= nullptr;
	iContext	= nullptr;
	iSwapChain	= nullptr;
	iRTV		= nullptr;
	pBB			= nullptr;
//---------------------------
	iDSV		= nullptr;
	iZBuffer	= nullptr;

	iVertexShader	= nullptr;
	iPixelShader	= nullptr;

	iConstBufferCamera	= nullptr;
	iConstBufferScene	= nullptr;

	for(int iter = 0; iter < 3; iter++){
		StarList[iter].iConstBuffer = nullptr;
		StarList[iter].iIndexBuffer = nullptr;
		StarList[iter].iVertexBuffer = nullptr;
	}
	StarList[0].Velocity = XMFLOAT3(RANDOM_VELOCITY, RANDOM_VELOCITY, RANDOM_VELOCITY);
	StarList[1].Velocity = XMFLOAT3(RANDOM_VELOCITY, RANDOM_VELOCITY, RANDOM_VELOCITY);
	StarList[2].Velocity = XMFLOAT3(RANDOM_VELOCITY, RANDOM_VELOCITY, RANDOM_VELOCITY);
	
	StarList[0].Rotation = XMFLOAT3(RANDOM_ROTATION, RANDOM_ROTATION, RANDOM_ROTATION);
	StarList[1].Rotation = XMFLOAT3(RANDOM_ROTATION, RANDOM_ROTATION, RANDOM_ROTATION);
	StarList[2].Rotation = XMFLOAT3(RANDOM_ROTATION, RANDOM_ROTATION, RANDOM_ROTATION);

	for(int iter = 0; iter < 3; iter++){
		CubeList[iter].iConstBuffer = nullptr;
		CubeList[iter].iIndexBuffer = nullptr;
		CubeList[iter].iVertexBuffer = nullptr;
	}
	CubeList[0].Velocity = XMFLOAT3(RANDOM_VELOCITY, RANDOM_VELOCITY, RANDOM_VELOCITY);
	CubeList[1].Velocity = XMFLOAT3(RANDOM_VELOCITY, RANDOM_VELOCITY, RANDOM_VELOCITY);
	CubeList[2].Velocity = XMFLOAT3(RANDOM_VELOCITY, RANDOM_VELOCITY, RANDOM_VELOCITY);
	
	CubeList[0].Rotation = XMFLOAT3(RANDOM_ROTATION, RANDOM_ROTATION, RANDOM_ROTATION);
	CubeList[1].Rotation = XMFLOAT3(RANDOM_ROTATION, RANDOM_ROTATION, RANDOM_ROTATION);
	CubeList[2].Rotation = XMFLOAT3(RANDOM_ROTATION, RANDOM_ROTATION, RANDOM_ROTATION);

	wireframe			= false;
	iRasterState		= nullptr;
	iRasterStateWire	= nullptr;

	iLayout	= nullptr;
}
Lab2::~Lab2(){ }

#pragma region HELPER FUNCTIONS
HRESULT Lab2::HelperSetup(HWND window, UINT uiWidth, UINT uiHeight){
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

//---------------------------

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

//---------------------------
	return S_OK;
}

HRESULT Lab2::CreateConstBuffer(ID3D11Buffer **pConstBuffer, UINT Stride, UINT Count){
	D3D11_BUFFER_DESC iConstBufferDesc;
	ZeroMemory(&iConstBufferDesc, sizeof(D3D11_BUFFER_DESC));
	iConstBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	iConstBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	iConstBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	iConstBufferDesc.ByteWidth = Stride * Count;
	iConstBufferDesc.StructureByteStride = Stride;

	HRESULT	hr = iDevice->CreateBuffer(&iConstBufferDesc, nullptr, pConstBuffer);
	assert(S_OK == hr);

	return S_OK;
}
HRESULT Lab2::CreateConstBuffer(ID3D11Buffer **pConstBuffer, UINT Stride, UINT Count, void* SubData){
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

HRESULT Lab2::MapData(ID3D11Buffer *pBuffer, void* pData, UINT Stride){
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
#pragma endregion

void Lab2::UpdateCamera(XTime xTime){
	float movementSpeed = 30.0f * (float)xTime.Delta();
	bool moved = false;
	float mX = 0, mY = 0, mZ = 0, rY = 0;
	if(GetAsyncKeyState('D')){
		mX+=movementSpeed;
		moved = true;
	}
	else if(GetAsyncKeyState('A')){
		mX-=movementSpeed;
		moved = true;
	}
	if(GetAsyncKeyState(VK_SPACE)){
		mY+=movementSpeed;
		moved = true;
	}
	else if(GetAsyncKeyState(VK_LSHIFT)){
		mY-=movementSpeed;
		moved = true;
	}
	if(GetAsyncKeyState('W')){
		mZ+=movementSpeed;
		moved = true;
	}
	else if(GetAsyncKeyState('S')){
		mZ-=movementSpeed;
		moved = true;
	}
	if(moved){
		XMMATRIX ViewMat = XMLoadFloat4x4(&Scene.ViewMatrix);
		ViewMat = XMMatrixInverse(NULL, ViewMat);
		XMMATRIX TransMat = XMMatrixTranslation(mX, mY, mZ);
		ViewMat = XMMatrixMultiply(TransMat, ViewMat);
		ViewMat = XMMatrixInverse(NULL, ViewMat);
		XMStoreFloat4x4(&Scene.ViewMatrix, ViewMat);//*/
		//Also works just condensed into one line
/*		XMStoreFloat4x4(&Scene.ViewMatrix, 
			XMMatrixInverse(NULL, XMMatrixMultiply(
			XMMatrixTranslation(mX, mY, mZ), XMMatrixInverse(
			NULL, XMLoadFloat4x4(&Scene.ViewMatrix)))));//*/
	}
}
void Lab2::UpdateGeometry(XTime xTime){
	float _Delta = (float)xTime.Delta();
	for(int iter = 0; iter < 3; iter++){
		StarList[iter].WorldMatrix._41 += (StarList[iter].Velocity.x * _Delta);
		StarList[iter].WorldMatrix._42 += (StarList[iter].Velocity.y * _Delta);
		StarList[iter].WorldMatrix._43 += (StarList[iter].Velocity.z * _Delta);
		XMStoreFloat4x4(&StarList[iter].WorldMatrix, XMMatrixMultiply(XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&StarList[iter].Rotation) * _Delta), XMLoadFloat4x4(&StarList[iter].WorldMatrix)));
	}
	for(int iter = 0; iter < 3; iter++){
		CubeList[iter].WorldMatrix._41 += (CubeList[iter].Velocity.x * _Delta);
		CubeList[iter].WorldMatrix._42 += (CubeList[iter].Velocity.y * _Delta);
		CubeList[iter].WorldMatrix._43 += (CubeList[iter].Velocity.z * _Delta);
		XMStoreFloat4x4(&CubeList[iter].WorldMatrix, XMMatrixMultiply(XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&CubeList[iter].Rotation) * _Delta), XMLoadFloat4x4(&CubeList[iter].WorldMatrix)));
	}
}
void Lab2::CheckGeometry(){
	for(int iter = 0; iter < 3; iter++){
		if(StarList[iter].WorldMatrix._41 > BOUNDARY || StarList[iter].WorldMatrix._41 < -BOUNDARY){
			StarList[iter].Velocity.x *= -1;
		}
		if(StarList[iter].WorldMatrix._42 > BOUNDARY || StarList[iter].WorldMatrix._42 < -BOUNDARY){
			StarList[iter].Velocity.y *= -1;
		}
		if(StarList[iter].WorldMatrix._43 > BOUNDARY || StarList[iter].WorldMatrix._43 < -BOUNDARY){
			StarList[iter].Velocity.z *= -1;
		}
	}

	for(int iter = 0; iter < 3; iter++){
		if(CubeList[iter].WorldMatrix._41 > BOUNDARY || CubeList[iter].WorldMatrix._41 < -BOUNDARY){
			CubeList[iter].Velocity.x *= -1;
		}
		if(CubeList[iter].WorldMatrix._42 > BOUNDARY || CubeList[iter].WorldMatrix._42 < -BOUNDARY){
			CubeList[iter].Velocity.y *= -1;
		}
		if(CubeList[iter].WorldMatrix._43 > BOUNDARY || CubeList[iter].WorldMatrix._43 < -BOUNDARY){
			CubeList[iter].Velocity.z *= -1;
		}
	}//*/
}

bool Lab2::Setup(HWND window){
	HRESULT hr = HelperSetup(window, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT);
	assert(S_OK == hr);
//---------------------------
	hr = iDevice->CreateVertexShader(&Trivial_VS_2, sizeof(Trivial_VS_2), nullptr, &iVertexShader);
	hr = iDevice->CreatePixelShader(&Trivial_PS_2, sizeof(Trivial_PS_2), nullptr, &iPixelShader);
	
	D3D11_INPUT_ELEMENT_DESC SimpleLayout[] = {
		{"COLOR",	 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"PADDING",	 0, DXGI_FORMAT_R32_FLOAT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	hr = iDevice->CreateInputLayout(SimpleLayout, 3, &Trivial_VS_2, sizeof(Trivial_VS_2), &iLayout);
	assert(S_OK == hr);

	//Create Constant Buffer
	hr = CreateConstBuffer(&iConstBufferCamera, sizeof(CAMERA), 1);
	assert(S_OK == hr);
	hr = CreateConstBuffer(&iConstBufferScene, sizeof(SCENE), 1);
	assert(S_OK == hr);

	for(int iter = 0; iter < 3; iter++){
		XMStoreFloat4x4(&StarList[iter].WorldMatrix, XMMatrixIdentity());
		hr = CreateConstBuffer(&StarList[iter].iConstBuffer, sizeof(XMFLOAT4X4), 1);
		assert(S_OK == hr);
	}
	for(int iter = 0; iter < 3; iter++){
		XMStoreFloat4x4(&CubeList[iter].WorldMatrix, XMMatrixIdentity());
		hr = CreateConstBuffer(&CubeList[iter].iConstBuffer, sizeof(XMFLOAT4X4), 1);
		assert(S_OK == hr);
	}
	//Grid Locations
	XMStoreFloat4x4(&GridPanels[0].WorldMatrix, XMMatrixTranslation(0.0f, 0.0f, -BOUNDARY));
	XMStoreFloat4x4(&GridPanels[1].WorldMatrix, XMMatrixTranslation(0.0f, 0.0f,  BOUNDARY));
	XMStoreFloat4x4(&GridPanels[2].WorldMatrix, XMMatrixTranslation(-BOUNDARY, 0.0f, 0.0f));
	XMStoreFloat4x4(&GridPanels[3].WorldMatrix, XMMatrixTranslation( BOUNDARY, 0.0f, 0.0f));
	XMStoreFloat4x4(&GridPanels[4].WorldMatrix, XMMatrixTranslation(0.0f,  BOUNDARY, 0.0f));
	XMStoreFloat4x4(&GridPanels[5].WorldMatrix, XMMatrixTranslation(0.0f, -BOUNDARY, 0.0f));
	//Grid Orientations
	XMStoreFloat4x4(&GridPanels[1].WorldMatrix, XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(180.0f)), XMLoadFloat4x4(&GridPanels[1].WorldMatrix)));
	XMStoreFloat4x4(&GridPanels[2].WorldMatrix, XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians( 90.0f)), XMLoadFloat4x4(&GridPanels[2].WorldMatrix)));
	XMStoreFloat4x4(&GridPanels[3].WorldMatrix, XMMatrixMultiply(XMMatrixRotationY(XMConvertToRadians(-90.0f)), XMLoadFloat4x4(&GridPanels[3].WorldMatrix)));
	XMStoreFloat4x4(&GridPanels[4].WorldMatrix, XMMatrixMultiply(XMMatrixRotationX(XMConvertToRadians( 90.0f)), XMLoadFloat4x4(&GridPanels[4].WorldMatrix)));
	XMStoreFloat4x4(&GridPanels[5].WorldMatrix, XMMatrixMultiply(XMMatrixRotationX(XMConvertToRadians(-90.0f)), XMLoadFloat4x4(&GridPanels[5].WorldMatrix)));
	for(int iter = 0; iter < 6; iter++){
		hr = CreateConstBuffer(&GridPanels[iter].iConstBuffer, sizeof(XMFLOAT4X4), 1);
		assert(S_OK == hr);
	}
	
//Geometry Computation
#pragma region TRIANGLE DATA
/*	SIMPLE_VERTEX Triangle[] = {
//					COLOR				POSITION
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3(-3, 0, 0) },
		{ XMFLOAT4(0, 1, 0, 1), XMFLOAT3( 0, 6, 0) },
		{ XMFLOAT4(0, 0, 1, 1), XMFLOAT3( 3, 0, 0) }
	};

	unsigned short TriangleIndex[] = { 0, 1, 2 };//*/
#pragma endregion
#pragma region CUBE DATA
	SIMPLE_VERTEX Cube[] = {
//						COLOR				POS
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3(-1,  1, -1) }, //0
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3( 1,  1, -1) }, //1
		{ XMFLOAT4(0, 1, 1, 1), XMFLOAT3( 1, -1, -1) }, //2
		{ XMFLOAT4(0, 1, 1, 1), XMFLOAT3(-1, -1, -1) }, //3
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3( 1,  1,  1) }, //4
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3(-1,  1,  1) }, //5
		{ XMFLOAT4(0, 1, 1, 1), XMFLOAT3(-1, -1,  1) }, //6
		{ XMFLOAT4(0, 1, 1, 1), XMFLOAT3( 1, -1,  1) }, //7
	};

	unsigned short CubeIndex[] = {
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
#pragma endregion
#pragma region STAR DATA
	SIMPLE_VERTEX Star[] = {
//						COLOR						POS
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3(-3,  6, -1) }, //0
		{ XMFLOAT4(1, 1, 0, 1), XMFLOAT3( 0, 15, -1) },	//1
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3( 3,  6, -1) },	//2
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3(12,  6, -1) },	//3
		{ XMFLOAT4(0, 1, 0, 1), XMFLOAT3( 5,  0, -1) },	//4
		{ XMFLOAT4(0, 0, 1, 1), XMFLOAT3( 7, -8, -1) },	//5
		{ XMFLOAT4(0, 1, 0, 1), XMFLOAT3( 0, -4, -1) },	//6
		{ XMFLOAT4(0, 0, 1, 1), XMFLOAT3(-7, -8, -1) },	//7
		{ XMFLOAT4(0, 1, 0, 1), XMFLOAT3(-5,  0, -1) },	//8
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3(-12, 6, -1) },	//9
		//Backside
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3(-3,  6,  1) },
		{ XMFLOAT4(1, 1, 0, 1), XMFLOAT3( 0, 15,  1) },
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3( 3,  6,  1) },
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3(12,  6,  1) },
		{ XMFLOAT4(0, 1, 0, 1), XMFLOAT3( 5,  0,  1) },
		{ XMFLOAT4(0, 0, 1, 1), XMFLOAT3( 7, -8,  1) },
		{ XMFLOAT4(0, 1, 0, 1), XMFLOAT3( 0, -4,  1) },
		{ XMFLOAT4(0, 0, 1, 1), XMFLOAT3(-7, -8,  1) },
		{ XMFLOAT4(0, 1, 0, 1), XMFLOAT3(-5,  0,  1) },
		{ XMFLOAT4(1, 0, 0, 1), XMFLOAT3(-12, 6,  1) } 
	};
	for(int starVertIter = 0; starVertIter < STAR_VERTEX_COUNT; starVertIter++){
		Star[starVertIter].Pos.x /= 2.5f;
		Star[starVertIter].Pos.y /= 2.5f;
		Star[starVertIter].Pos.z /= 2.5f;
	}

	unsigned short StarIndex[] = {
		0, 1, 2,	//Top
		2, 3, 4,	//Right
		4, 5, 6,	//Bottom Right
		6, 7, 8,	//Bottom Left
		8, 9, 0,	//Left
		0, 6, 8,	//Center Left
		0, 2, 6,	//Center
		2, 4, 6,	//Center Right
		//Second Star
		12, 11, 10,	//Top
		14, 13, 12,	//Right
		16, 15, 14,	//Bottom Right
		18, 17, 16,	//Bottom Left
		10, 19, 18,	//Left
		18, 16, 10,	//Center Left
		16, 12, 10,	//Center
		16, 14, 12,	//Center Right
		//Side Connections
		0, 19, 10,
		0, 10, 1,
		1, 10, 11,
		1, 11, 2,
		2, 11, 12,
		2, 12, 3,
		3, 12, 13,
		3, 13, 4,
		4, 13, 14,
		4, 14, 5,
		5, 14, 15,
		5, 15, 6,
		6, 15, 16,
		6, 16, 7,
		7, 16, 17,
		7, 17, 8,
		8, 17, 18,
		8, 18, 9,
		9, 18, 19,
		9, 19, 0,
	};//*/
#pragma endregion
#pragma region TRIANGLE GRID
/*	SIMPLE_VERTEX gridPanel[GRID_ROWS * GRID_COLUMNS];

//Vertices
	for(int rowIter = 0; rowIter < GRID_ROWS; rowIter++){
		for(int colIter = 0; colIter < GRID_COLUMNS; colIter++){
			int index = (rowIter * GRID_COLUMNS) + colIter;
			gridPanel[index] = SIMPLE_VERTEX();
			gridPanel[index].Color = XMFLOAT4(0, 1, 0, 1);
			gridPanel[index].Pos = XMFLOAT3((float)colIter - (GRID_COLUMNS / 2), -(float)(rowIter - (GRID_ROWS / 2)), 0);
		}
	}//*/
/*
	XMFLOAT3(-2, 2, 0);	0	   XMFLOAT3(-2, 1, 0);	5	 XMFLOAT3(-2, 0, 0);	10
	XMFLOAT3(-1, 2, 0);	1	   XMFLOAT3(-1, 1, 0);	6	 XMFLOAT3(-1, 0, 0);	11
	XMFLOAT3( 0, 2, 0);	2	   XMFLOAT3( 0, 1, 0);	7	 XMFLOAT3( 0, 0, 0);	12
	XMFLOAT3( 1, 2, 0);	3	   XMFLOAT3( 1, 1, 0);	8	 XMFLOAT3( 1, 0, 0);	13
	XMFLOAT3( 2, 2, 0);	4	   XMFLOAT3( 2, 1, 0);	9	 XMFLOAT3( 2, 0, 0);	14
	
	XMFLOAT3(-2,-1, 0);	15	   XMFLOAT3(-2,-2, 0);	20
	XMFLOAT3(-1,-1, 0);	16	   XMFLOAT3(-1,-2, 0);	21
	XMFLOAT3( 0,-1, 0);	17	   XMFLOAT3( 0,-2, 0);	22
	XMFLOAT3( 1,-1, 0);	18	   XMFLOAT3( 1,-2, 0);	23
	XMFLOAT3( 2,-1, 0);	19	   XMFLOAT3( 2,-2, 0);	24
//*/

/*	unsigned short gridPanelIndices[((GRID_COLUMNS - 1) * 6) * (GRID_ROWS - 1)];
	int index = 0;
//Indices
	for(int rowIter = 0; rowIter < GRID_ROWS - 1; rowIter++){
		int rowStart = rowIter * GRID_COLUMNS;
		int nxtRowStart = (rowIter + 1) * GRID_COLUMNS;
		for(int colIter = 0; colIter < GRID_COLUMNS - 1; colIter++){
			gridPanelIndices[index    ] = rowStart + colIter;				// 0 *---* 1
			gridPanelIndices[index + 1] = rowStart + colIter + 1;			//   | \ |
			gridPanelIndices[index + 2] = nxtRowStart + colIter + 1;		//   |  \|
			gridPanelIndices[index + 3] = rowStart + colIter;				// 5 *---* 6
			gridPanelIndices[index + 4] = nxtRowStart + colIter + 1;
			gridPanelIndices[index + 5] = nxtRowStart + colIter;
			index += 6;
		}
	}//*/
#pragma endregion
#pragma region LINE GRID
	SIMPLE_VERTEX lineGridPanel[LN_GRD_VERT_COUNT];
	for(int iter = 0; iter < LN_GRD_VERT_COUNT; iter++){
		lineGridPanel[iter] = SIMPLE_VERTEX();
		lineGridPanel[iter].Color = XMFLOAT4(0, 1, 0, 1);
	}

//Vertices
	float HalfBoundary = (LN_GRD_BNDRY / 2.0f);
	int BtmStart = LN_GRD_COLUMNS + (2 * (LN_GRD_ROWS - 2));
	//float colYOffset = (-LN_GRD_COLUMNS / 2.0f) + colIter) * LN_GRD_STEP_X;
	for(int colIter = 0; colIter < LN_GRD_COLUMNS; colIter++){
		lineGridPanel[colIter].Pos			  = XMFLOAT3(HalfBoundary - (colIter * LN_GRD_STEP_X), HalfBoundary, 0);
		lineGridPanel[BtmStart + colIter].Pos = XMFLOAT3(HalfBoundary - (colIter * LN_GRD_STEP_X),-HalfBoundary, 0);
	}

	int MidStart = LN_GRD_COLUMNS;
	//float rowXOffset = (+/-LN_GRD_COLUMNS / 2.0f) * LN_GRD_STEP_X;
	for(int rowIter = 0; rowIter < LN_GRD_ROWS - 2; rowIter++){
		lineGridPanel[MidStart + (rowIter * 2)].Pos		= XMFLOAT3(-HalfBoundary, HalfBoundary - ((rowIter + 1) * LN_GRD_STEP_Y), 0);
		lineGridPanel[MidStart + (rowIter * 2) + 1].Pos = XMFLOAT3( HalfBoundary, HalfBoundary - ((rowIter + 1) * LN_GRD_STEP_Y), 0);
	}
/*
	XMFLOAT3(-2, 2, 0);	0
	XMFLOAT3(-1, 2, 0);	1
	XMFLOAT3( 0, 2, 0);	2
	XMFLOAT3( 1, 2, 0);	3
	XMFLOAT3( 2, 2, 0);	4

	XMFLOAT3(-2, 1, 0);	5		XMFLOAT3( 2, 1, 0);	6
	XMFLOAT3(-2, 0, 0);	7		XMFLOAT3( 2, 0, 0);	8
	XMFLOAT3(-2,-1, 0);	9		XMFLOAT3( 2,-1, 0);	10

	XMFLOAT3(-2,-2, 0);	11
	XMFLOAT3(-1,-2, 0);	12
	XMFLOAT3( 0,-2, 0);	13
	XMFLOAT3( 1,-2, 0);	14
	XMFLOAT3( 2,-2, 0);	15
//*/
//Indices
	unsigned short lineGridPanelIndices[LN_GRD_IND_COUNT];
	int IndexIter = 0;
	lineGridPanelIndices[IndexIter++] = 0;
	lineGridPanelIndices[IndexIter++] = LN_GRD_COLUMNS - 1;
	int index = LN_GRD_COLUMNS;
	for(int rowIter = 1; rowIter < LN_GRD_ROWS - 1; rowIter++){
		lineGridPanelIndices[IndexIter++] = index;
		lineGridPanelIndices[IndexIter++] = index + 1;
		index += 2;
	}
	lineGridPanelIndices[IndexIter++] = index;
	lineGridPanelIndices[IndexIter++] = index + LN_GRD_COLUMNS - 1;

	index = LN_GRD_COLUMNS + ((LN_GRD_ROWS - 2) * 2);
	for(int colIter = 0; colIter < LN_GRD_COLUMNS; colIter++){
		lineGridPanelIndices[IndexIter++] = colIter;
		lineGridPanelIndices[IndexIter++] = index + colIter;
	}
/*
	0, 4,		0, 11
	5, 6,		1, 12
	7, 8,		2, 13
	9, 10,		3, 14
	11, 15		4, 15
//*/

#pragma endregion
	D3D11_BUFFER_DESC BufferDesc;
	D3D11_SUBRESOURCE_DATA InitData;
#pragma region VERTEX BUFFERS
	ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
	BufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.StructureByteStride = sizeof(SIMPLE_VERTEX);

	//Star Vertex Buffers
	InitData.pSysMem = &Star;
	for(int iter = 0; iter < 3; iter++){
		StarList[iter].VertexCount = STAR_VERTEX_COUNT;
		StarList[iter].IndexCount  = STAR_INDEX_COUNT;
		BufferDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * StarList[iter].VertexCount;
		hr = iDevice->CreateBuffer(&BufferDesc, &InitData, &StarList[iter].iVertexBuffer);
		assert(S_OK == hr);
	}
	//Cube Vertex Buffers
	InitData.pSysMem = &Cube;
	for(int iter = 0; iter < 3; iter++){
		CubeList[iter].VertexCount = CUBE_VERTEX_COUNT;
		CubeList[iter].IndexCount  = CUBE_INDEX_COUNT;
		BufferDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * CubeList[iter].VertexCount;
		hr = iDevice->CreateBuffer(&BufferDesc, &InitData, &CubeList[iter].iVertexBuffer);
		assert(S_OK == hr);
		if(iter >= 1){
			for(int CubeVertIter = 0; CubeVertIter < CUBE_VERTEX_COUNT; CubeVertIter++){
				Cube[CubeVertIter].Pos.x /= 3;
				Cube[CubeVertIter].Pos.y /= 3;
				Cube[CubeVertIter].Pos.z /= 3;
			}
		}
	}
	//Grid Vertex Buffers
	InitData.pSysMem = &lineGridPanel;
	for(int iter = 0; iter < 6; iter++){
		GridPanels[iter].VertexCount = LN_GRD_VERT_COUNT;
		GridPanels[iter].IndexCount = LN_GRD_IND_COUNT;
		BufferDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * GridPanels[iter].VertexCount;
		hr = iDevice->CreateBuffer(&BufferDesc, &InitData, &GridPanels[iter].iVertexBuffer);
		assert(S_OK == hr);
	}//*/
/*	InitData.pSysMem = &gridPanel;
	for(int iter = 0; iter < 6; iter++){
		GridPanels[iter].VertexCount = GRID_ROWS * GRID_COLUMNS;
		GridPanels[iter].IndexCount = ((GRID_COLUMNS - 1) * 6) * (GRID_ROWS - 1);
		BufferDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * GridPanels[iter].VertexCount;
		hr = iDevice->CreateBuffer(&BufferDesc, &InitData, &GridPanels[iter].iVertexBuffer);
		assert(S_OK == hr);
	}//*/
#pragma endregion
#pragma region INDEX BUFFERS
	BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	BufferDesc.StructureByteStride = sizeof(unsigned short);
	
	//Star Index Buffers
	InitData.pSysMem = &StarIndex;
	for(int iter = 0; iter < 3; iter++){
		BufferDesc.ByteWidth = sizeof(unsigned short) * StarList[iter].IndexCount;
		hr = iDevice->CreateBuffer(&BufferDesc, &InitData, &StarList[iter].iIndexBuffer);
		assert(S_OK == hr);
	}
	//Cube Index Buffers
	InitData.pSysMem = &CubeIndex;
	for(int iter = 0; iter < 3; iter++){
		BufferDesc.ByteWidth = sizeof(unsigned short) * CubeList[iter].IndexCount;
		hr = iDevice->CreateBuffer(&BufferDesc, &InitData, &CubeList[iter].iIndexBuffer);
		assert(S_OK == hr);
	}
	//Grid Index Buffers
	InitData.pSysMem = &lineGridPanelIndices; //&gridPanelIndices;
	for(int iter = 0; iter < 6; iter++){
		BufferDesc.ByteWidth = sizeof(unsigned short) * GridPanels[iter].IndexCount;
		hr = iDevice->CreateBuffer(&BufferDesc, &InitData, &GridPanels[iter].iIndexBuffer);
		assert(S_OK == hr);
	}//*/
#pragma endregion

//Rasterizer States for Wireframe
	D3D11_RASTERIZER_DESC RasterDesc;
	ZeroMemory(&RasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	RasterDesc.FillMode = D3D11_FILL_SOLID;
	RasterDesc.CullMode = D3D11_CULL_BACK;
	RasterDesc.DepthClipEnable = true;;

	//Solid State
	hr = iDevice->CreateRasterizerState(&RasterDesc, &iRasterState);
	assert(S_OK == hr);
	
	//Wireframe State
	RasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	RasterDesc.CullMode = D3D11_CULL_NONE;

	hr = iDevice->CreateRasterizerState(&RasterDesc, &iRasterStateWire);
	assert(S_OK == hr);
//---------------------------

	ZeroMemory(&Camera, sizeof(OBJECT));
	ZeroMemory(&Scene, sizeof(SCENE));
	XMStoreFloat4x4(&Camera.WorldMatrix, XMMatrixLookAtLH(XMLoadFloat3(&XMFLOAT3(22.0f, 20.0f, 22.0f)), XMLoadFloat3(&XMFLOAT3(0, 0, 0)), XMLoadFloat3(&XMFLOAT3(0, 1.0f, 0))));
	XMStoreFloat4x4(&Scene.ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&Scene.ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(75.0f), 1, 1.0f, 1000.0f));

	return true;
}
bool Lab2::Run(XTime xTime){
	iContext->OMSetRenderTargets(1, &iRTV, iDSV);
	iContext->RSSetViewports(1, &Viewport);
	float CLEAR_COLOR[] = {0, 0, 0, 1};
	iContext->ClearRenderTargetView(iRTV, CLEAR_COLOR);
//---------------------------
	iContext->ClearDepthStencilView(iDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

	UpdateCamera(xTime);
	UpdateGeometry(xTime);
	CheckGeometry();

//==Wireframe practice=======
	if(GetAsyncKeyState('J') & 0x8000){
		wireframe = !wireframe;
	}
	if(!wireframe){
		iContext->RSSetState(iRasterState);
	}
	else{
		iContext->RSSetState(iRasterStateWire);
	}
//===========================

	//Mapping data to the GPU
	MapData(iConstBufferCamera, &Camera, sizeof(CAMERA));
	MapData(iConstBufferScene, &Scene, sizeof(SCENE));
	for(int iter = 0; iter < 3; iter++){
		MapData(StarList[iter].iConstBuffer, &StarList[iter].WorldMatrix, sizeof(XMFLOAT4X4));
	}
	for(int iter = 0; iter < 3; iter++){
		MapData(CubeList[iter].iConstBuffer, &CubeList[iter].WorldMatrix, sizeof(XMFLOAT4X4));
	}
	for(int iter = 0; iter < 6; iter++){
		MapData(GridPanels[iter].iConstBuffer, &GridPanels[iter].WorldMatrix, sizeof(XMFLOAT4X4));
	}
	
	iContext->VSSetConstantBuffers(0, 1, &iConstBufferCamera);
	iContext->VSSetConstantBuffers(1, 1, &iConstBufferScene);
	iContext->VSSetShader(iVertexShader, nullptr, NULL);
	iContext->PSSetShader(iPixelShader, nullptr, NULL);
	iContext->IASetInputLayout(iLayout);
	iContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	UINT Strides = sizeof(SIMPLE_VERTEX);
	UINT Offset = 0;
	
	//Stars
	for(int iter = 0; iter < 3; iter++){
		iContext->IASetVertexBuffers(0, 1, &StarList[iter].iVertexBuffer, &Strides, &Offset);
		iContext->IASetIndexBuffer(StarList[iter].iIndexBuffer, DXGI_FORMAT_R16_UINT, NULL);
		iContext->VSSetConstantBuffers(2, 1, &StarList[iter].iConstBuffer);
		iContext->DrawIndexed(StarList[iter].IndexCount, 0, NULL);
	}//*/

	//Cubes
	for(int iter = 0; iter < 3; iter++){
		iContext->IASetVertexBuffers(0, 1, &CubeList[iter].iVertexBuffer, &Strides, &Offset);
		iContext->IASetIndexBuffer(CubeList[iter].iIndexBuffer, DXGI_FORMAT_R16_UINT, NULL);
		iContext->VSSetConstantBuffers(2, 1, &CubeList[iter].iConstBuffer);
		iContext->DrawIndexed(CubeList[iter].IndexCount, 0, NULL);
	}//*/

	//Grid
	iContext->RSSetState(iRasterStateWire);
	iContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	for(int iter = 0; iter < 6; iter++){
		iContext->IASetVertexBuffers(0, 1, &GridPanels[iter].iVertexBuffer, &Strides, &Offset);
		iContext->IASetIndexBuffer(GridPanels[iter].iIndexBuffer, DXGI_FORMAT_R16_UINT, NULL);
		iContext->VSSetConstantBuffers(2, 1, &GridPanels[iter].iConstBuffer);
		iContext->DrawIndexed(GridPanels[iter].IndexCount, 0, NULL);
	}
	iContext->RSSetState(iRasterState);
//---------------------------
	iSwapChain->Present(0, 0);

	return true;
}
bool Lab2::ShutDown(){
	iContext->ClearState();
	iRTV->Release();
	pBB->Release();
//---------------------------
	iZBuffer->Release();
	iDSV->Release();

	iVertexShader->Release();
	iPixelShader->Release();

	iConstBufferCamera->Release();
	iConstBufferScene->Release();

	for(int iter = 0; iter < 3; iter++){
		StarList[iter].iConstBuffer->Release();
		StarList[iter].iIndexBuffer->Release();
		StarList[iter].iVertexBuffer->Release();
	}
	for(int iter = 0; iter < 3; iter++){
		CubeList[iter].iConstBuffer->Release();
		CubeList[iter].iIndexBuffer->Release();
		CubeList[iter].iVertexBuffer->Release();
	}
	for(int iter = 0; iter < 6; iter++){
		GridPanels[iter].iConstBuffer->Release();
		GridPanels[iter].iIndexBuffer->Release();
		GridPanels[iter].iVertexBuffer->Release();

	}

	iRasterState->Release();
	iRasterStateWire->Release();

	iLayout->Release();
//---------------------------
	iSwapChain->Release();
	iContext->Release();
	iDevice->Release();

	return true;
}
