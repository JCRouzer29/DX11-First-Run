#include "Object.h"

Object::Object(){
	iConstBuffer = nullptr;
	iVertexBuffer = nullptr;
	iIndexBuffer = nullptr;
}
Object::~Object(){ }

void Object::SetPosition(XMFLOAT3 vPos){
	WorldMatrix._41 = vPos.x;
	WorldMatrix._42 = vPos.y;
	WorldMatrix._43 = vPos.z;
}


HRESULT Object::MapConstBuffer(ID3D11DeviceContext *iContext){
	D3D11_MAPPED_SUBRESOURCE MapResource;
	ZeroMemory(&MapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	MapResource.pData = &WorldMatrix;
	MapResource.DepthPitch = sizeof(XMFLOAT4X4);
	MapResource.RowPitch = sizeof(XMFLOAT4X4);

	HRESULT hr;
	hr = iContext->Map(iConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &MapResource);
	memcpy(MapResource.pData, &WorldMatrix, sizeof(XMFLOAT4X4));
	iContext->Unmap(iConstBuffer, 0);
	assert(S_OK == hr);

	return S_OK;
}

void Object::Release(){
	ReleaseConstBuffer();
	ReleaseVertexBuffer();
	ReleaseIndexBuffer();
}

void Object::Update(XTime xTime){
	float _Delta = (float)xTime.Delta();
	//Update position based on Velocity
	WorldMatrix._41 += (Velocity.x * _Delta);
	WorldMatrix._42 += (Velocity.y * _Delta);
	WorldMatrix._43 += (Velocity.z * _Delta);

	XMStoreFloat4x4(&WorldMatrix, XMMatrixRotationRollPitchYawFromVector(
		XMLoadFloat3(&Rotation) * _Delta) * XMLoadFloat4x4(&WorldMatrix));
}
