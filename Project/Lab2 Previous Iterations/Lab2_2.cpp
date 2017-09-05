//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include "Lab2.h"

Lab2::Lab2(){
	iDevice = nullptr;
	iContext = nullptr;
	iSwapChain = nullptr;
	iConstBufferGeometry = nullptr;
}
Lab2::~Lab2(){}

bool Lab2::Setup(HWND window){
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
	
	hr = iDevice->CreateVertexShader(&Trivial_VS_2, sizeof(Trivial_VS_2), nullptr, &iVertexShader);
	hr = iDevice->CreatePixelShader(&Trivial_PS_2, sizeof(Trivial_PS_2), nullptr, &iPixelShader);
	
	D3D11_INPUT_ELEMENT_DESC SimpleLayout[] = {
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"PADDING" , 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	hr = iDevice->CreateInputLayout(SimpleLayout, 3, &Trivial_VS_2, sizeof(Trivial_VS_2), &iLayout);

	D3D11_BUFFER_DESC ConstBufferDesc;
	ZeroMemory(&ConstBufferDesc, sizeof(ConstBufferDesc));
	ConstBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstBufferDesc.ByteWidth = sizeof(Triangle);
	ConstBufferDesc.StructureByteStride = sizeof(SIMPLE_VERTEX);
	ConstBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ConstBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = iDevice->CreateBuffer(&ConstBufferDesc, nullptr, &iConstBufferGeometry);

//Geometry Computation
	Triangle[0].Pos = XMFLOAT3(1, 0, 0);
	Triangle[1].Pos = XMFLOAT3(0, 2, 0);
	Triangle[2].Pos = XMFLOAT3(-1,0, 0);

	D3D11_BUFFER_DESC VertexBufferDesc;


	hr = iDevice->CreateBuffer(&VertexBufferDesc, &Triangle, &iVertexBuffer);

	D3D11_TEXTURE2D_DESC ZBufferDesc;
	ZeroMemory(&ZBufferDesc, sizeof(ZBufferDesc));
	ZBufferDesc.Width = SwapChainDesc.BufferDesc.Width;
	ZBufferDesc.Height = SwapChainDesc.BufferDesc.Height;
	ZBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	ZBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	ZBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ZBufferDesc.CPUAccessFlags = 0;
	ZBufferDesc.ArraySize = 1;
	ZBufferDesc.MipLevels = 1;
	ZBufferDesc.SampleDesc.Count = 1;

	hr = iDevice->CreateTexture2D(&ZBufferDesc, nullptr, &iZBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	ZeroMemory(&DSVDesc, sizeof(DSVDesc));
	DSVDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DSVDesc.Texture2D.MipSlice = 0;
	hr = iDevice->CreateDepthStencilView(iZBuffer, &DSVDesc, &iDSV);

	return true;
}
bool Lab2::Run(XTime xTime){
	UINT Strides = sizeof(SIMPLE_VERTEX);
	UINT Offset = 0;
	
	iContext->OMSetRenderTargets(1, &iRTV, iDSV);
	
	iContext->RSSetViewports(1, &Viewport);
	
	float color[] = {0, 0, 255, 1};
	iContext->ClearRenderTargetView(iRTV, color);
	iContext->ClearDepthStencilView(iDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	D3D11_MAPPED_SUBRESOURCE mapSub;
	mapSub.pData = &Triangle;
	mapSub.DepthPitch = sizeof(SIMPLE_VERTEX);
	mapSub.RowPitch = sizeof(SIMPLE_VERTEX);
	iContext->Map(iConstBufferGeometry, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mapSub);
	memcpy(mapSub.pData, &Triangle, sizeof(Triangle));
	iContext->Unmap(iConstBufferGeometry, 0);
	iContext->VSSetConstantBuffers(1, 1, &iConstBufferGeometry);

	iContext->IASetVertexBuffers(0, 1, &iVertexBuffer, &Strides, &Offset);
	iContext->VSSetShader(iVertexShader, nullptr, NULL);
	iContext->PSSetShader(iPixelShader, nullptr, NULL);
	
	iContext->IASetInputLayout(iLayout);
	iContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	iContext->Draw(3, 0);

	iSwapChain->Present(0, 0);

	return true;
}
bool Lab2::ShutDown(){
	iContext->ClearState();
	iRTV->Release();
	pBB->Release();
	iZBuffer->Release();
	iDSV->Release();

	iVertexShader->Release();
	iPixelShader->Release();

	iConstBufferGeometry->Release();

	iLayout->Release();
	
	iSwapChain->Release();
	iContext->Release();
	iDevice->Release();

	return true;
}
