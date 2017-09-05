//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include "Lab1.h"

Lab1::Lab1(){
	iDevice = nullptr;
	iContext = nullptr;
	iSwapChain = nullptr;

	iRTV = nullptr;
	pBB = nullptr;

	iCircleBuffer = nullptr;

	iGridVertBuffer = nullptr;
	iGridConstBuffer = nullptr;

	iVertexShader = nullptr;
	iPixelShader = nullptr;
	iLayout = nullptr;
	iConstBuffer = nullptr;
}
Lab1::~Lab1(){ }

bool Lab1::Setup(HWND window){
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.BufferDesc.Width = BACKBUFFER_WIDTH;
	SwapChainDesc.BufferDesc.Height = BACKBUFFER_HEIGHT;
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

	iSwapChain->GetBuffer(0, __uuidof(pBB), reinterpret_cast<void**>(&pBB));
	hr = iDevice->CreateRenderTargetView(pBB, NULL, &iRTV);
	
	Viewport.MinDepth = 0.0f;
	Viewport.MaxDepth = 1.0f;
	Viewport.Height = (float)SwapChainDesc.BufferDesc.Height;
	Viewport.Width = (float)SwapChainDesc.BufferDesc.Width;
	Viewport.TopLeftX = Viewport.TopLeftY = 0;
	
	SIMPLE_VERTEX circle[CIRCLE_VERT_COUNT];
	for(int iter = 0; iter < CIRCLE_VERT_COUNT; iter++){
		circle[iter].pos.x = XMScalarCos(XMConvertToRadians((float)iter));
		circle[iter].pos.y = XMScalarSin(XMConvertToRadians((float)iter));
	}
	
	for(int iter = 0; iter < CIRCLE_VERT_COUNT; iter++){
		circle[iter].pos.x *= 0.2f;
		circle[iter].pos.y *= 0.2f;
	}

	D3D11_BUFFER_DESC iCircleBufferDesc;
	ZeroMemory(&iCircleBufferDesc, sizeof(iCircleBufferDesc));
	iCircleBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	iCircleBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	iCircleBufferDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * CIRCLE_VERT_COUNT;
	iCircleBufferDesc.StructureByteStride = sizeof(SIMPLE_VERTEX);
	
	D3D11_SUBRESOURCE_DATA CircleData;
	ZeroMemory(&CircleData, sizeof(CircleData));
	CircleData.pSysMem = &circle;
//	CircleData.SysMemPitch;
//	CircleData.SysMemSlicePitch;
	
	hr = iDevice->CreateBuffer(&iCircleBufferDesc, &CircleData, &iCircleBuffer);
	
	GridVertexCount = 1200;
	SIMPLE_VERTEX Grid[1200];

	float baseX = -1.0f, baseY = 1.0f;
	for(int Ver = 0; Ver < 20; Ver++){//Each Row from the Column
		const int VIndex = Ver * 60;

		if(Ver % 2 == 0){//Offset every even Row
			baseX += 0.1f;
		}
		for(int Hor = 0; Hor < 10; Hor++){//Across the Row
			const int HIndex = Hor * 6;

			Grid[VIndex + HIndex + 0].pos.x = baseX;
			Grid[VIndex + HIndex + 0].pos.y = baseY;
			
			Grid[VIndex + HIndex + 1].pos.x = baseX + 0.1f;
			Grid[VIndex + HIndex + 1].pos.y = baseY;
			
			Grid[VIndex + HIndex + 2].pos.x = baseX;
			Grid[VIndex + HIndex + 2].pos.y = baseY - 0.1f;
			
			Grid[VIndex + HIndex + 3].pos.x = baseX;
			Grid[VIndex + HIndex + 3].pos.y = baseY - 0.1f;
			
			Grid[VIndex + HIndex + 4].pos.x = baseX + 0.1f;
			Grid[VIndex + HIndex + 4].pos.y = baseY;
			
			Grid[VIndex + HIndex + 5].pos.x = baseX + 0.1f;
			Grid[VIndex + HIndex + 5].pos.y = baseY - 0.1f;

			baseX += 0.2f;
		}
		baseX = -1.0f;
		baseY -= 0.1f;
	}

#pragma region Tried
/*	// TODO: PART 5 STEP 2a
	GridVertexCount = 231;
	SIMPLE_VERTEX Grid[231];
	
	// TODO: PART 5 STEP 2b
	XMFLOAT3 left = XMFLOAT3(-1, 10, 0);
	XMFLOAT3 right = XMFLOAT3(10, 0, 0);
	float x = -1.0f, y = 1.0f;
	for(int vertical = 0; vertical < 20; vertical++){
		for(int horizontal = 0; horizontal < 10; horizontal++){
			//Increment the left
			left.x++;
			left.z++;

			//Add the left

			//Update the right
			right.x++;
			left.y++;
			right.y = left.y;
			right.z = left.z;

			//Add the right

			x += 0.1f;
		}
		//Increment the left
		left.x++;
		left.y++;
		left.z++;

		y -= 0.1f;
		x = -1.0f;
	}//*/
#pragma endregion
	
	iCircleBufferDesc.ByteWidth = sizeof(SIMPLE_VERTEX) * 1200;
	CircleData.pSysMem = &Grid;
	hr = iDevice->CreateBuffer(&iCircleBufferDesc, &CircleData, &iGridVertBuffer);
	
	hr = iDevice->CreateVertexShader(&Trivial_VS, sizeof(Trivial_VS), nullptr, &iVertexShader);
	hr = iDevice->CreatePixelShader(&Trivial_PS, sizeof(Trivial_PS), nullptr, &iPixelShader);
	
	D3D11_INPUT_ELEMENT_DESC SimpleLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	hr = iDevice->CreateInputLayout(SimpleLayout, 1, &Trivial_VS, sizeof(Trivial_VS), &iLayout);

	D3D11_BUFFER_DESC ConstBufferDesc;
	ZeroMemory(&ConstBufferDesc, sizeof(ConstBufferDesc));
	ConstBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstBufferDesc.ByteWidth = sizeof(SEND_TO_VRAM) * 361;
	ConstBufferDesc.StructureByteStride = sizeof(SEND_TO_VRAM);
	ConstBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = iDevice->CreateBuffer(&ConstBufferDesc, nullptr, &iConstBuffer);

	ConstBufferDesc.ByteWidth = sizeof(SEND_TO_VRAM) * 1200;
	hr= iDevice->CreateBuffer(&ConstBufferDesc, nullptr, &iGridConstBuffer);

	toShader.Offset = XMFLOAT2(0, 0);
	toShader.Color = XMFLOAT4(1, 1, 0, 1);

	GridToShader.Offset.x = 0.0f;
	GridToShader.Offset.y = 0.0f;
	GridToShader.Color.x = 0.0f;
	GridToShader.Color.y = 0.0f;
	GridToShader.Color.z = 0.0f;
	GridToShader.Color.w = 1.0f;

	HMove = VMove = 1.0f;

	return true;
}
bool Lab1::Run(XTime xTime){
	toShader.Offset.x += HMove * (float)xTime.Delta();
	toShader.Offset.y += VMove * (float)(0.5f * xTime.Delta());
	
	if(toShader.Offset.x > 1){
		HMove = -1.0f;
	}
	else if(toShader.Offset.x < -1){
		HMove = 1.0f;
	}
	if(toShader.Offset.y > 1){
		VMove = -1.0f;
	}
	else if(toShader.Offset.y < -1){
		VMove = 1.0f;
	}
	
	UINT Strides = sizeof(SIMPLE_VERTEX);
	UINT Offset = 0;

	iContext->OMSetRenderTargets(1, &iRTV, NULL);
	
	iContext->RSSetViewports(1, &Viewport);
	
	float color[] = {0, 0, 255, 1};
	iContext->ClearRenderTargetView(iRTV, color);
	
	D3D11_MAPPED_SUBRESOURCE GridMap;
	ZeroMemory(&GridMap, sizeof(GridMap));
	GridMap.pData = &GridToShader;
	GridMap.DepthPitch = sizeof(SEND_TO_VRAM);
	GridMap.RowPitch = sizeof(SEND_TO_VRAM);
	
	iContext->Map(iGridConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &GridMap);
	memcpy(GridMap.pData, &GridToShader, sizeof(GridToShader));
	iContext->Unmap(iGridConstBuffer, 0);
	
	iContext->VSSetConstantBuffers(0, 1, &iGridConstBuffer);
	iContext->IASetVertexBuffers(0, 1, &iGridVertBuffer, &Strides, &Offset);
	iContext->VSSetShader(iVertexShader, nullptr, NULL);
	iContext->PSSetShader(iPixelShader, nullptr, NULL);
	iContext->IASetInputLayout(iLayout);
	iContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	iContext->Draw(1200, 0);
	
	
	D3D11_MAPPED_SUBRESOURCE map;
	ZeroMemory(&map, sizeof(map));
	map.pData = &toShader;
	map.DepthPitch = sizeof(SEND_TO_VRAM);
	map.RowPitch = sizeof(SEND_TO_VRAM);

	iContext->Map(iConstBuffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, NULL, &map);
	memcpy(map.pData, &toShader, sizeof(toShader));
	iContext->Unmap(iConstBuffer, 0);
	
	iContext->VSSetConstantBuffers(0, 1, &iConstBuffer);
	
//	UINT Strides = sizeof(SIMPLE_VERTEX);
//	UINT Offset = 0;
	iContext->IASetVertexBuffers(0, 1, &iCircleBuffer, &Strides, &Offset);
	
	iContext->VSSetShader(iVertexShader, nullptr, NULL);
	iContext->PSSetShader(iPixelShader, nullptr, NULL);
	
	iContext->IASetInputLayout(iLayout);
	
	iContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	
	iContext->Draw(CIRCLE_VERT_COUNT, 0);
	
	iSwapChain->Present(0, 0);

	return true;
}
bool Lab1::ShutDown(){
	iContext->ClearState();
	iRTV->Release();
	pBB->Release();
	iCircleBuffer->Release();
	iGridVertBuffer->Release();
	iConstBuffer->Release();
	iGridConstBuffer->Release();
	iVertexShader->Release();
	iPixelShader->Release();
	iLayout->Release();

	iSwapChain->Release();
	iContext->Release();
	iDevice->Release();

	return true;
}
