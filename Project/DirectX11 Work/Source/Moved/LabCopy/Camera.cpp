#include "Camera.h"

Camera::Camera(){
	iConstBuffer = nullptr;
}
Camera::~Camera(){ }

void Camera::Update(XTime xTime){
	SnapMove(xTime);
	//Rotate(xTime);
}

//Overall this kind of works
//		Could stand to be improved upon
void Camera::SnapMove(XTime xTime){
	bool moved = false;
	XMFLOAT3 move = XMFLOAT3(0, 0, 0);

	if(GetAsyncKeyState('D')){
		move.x += CAMERA_MOVE_X;
		moved = true;
	}
	else if(GetAsyncKeyState('A')){
		move.x -= CAMERA_MOVE_X;
		moved = true;
	}
	if(GetAsyncKeyState(VK_SPACE)){
		move.y += CAMERA_MOVE_Y;
		moved = true;
	}
	else if(GetAsyncKeyState(VK_LSHIFT)){
		move.y -= CAMERA_MOVE_Y;
		moved = true;
	}
	if(GetAsyncKeyState('W')){
		move.z += CAMERA_MOVE_Z;
		moved = true;
	}
	else if(GetAsyncKeyState('S')){
		move.z -= CAMERA_MOVE_Z;
		moved = true;
	}

	if(moved){
		XMMATRIX ViewMat = XMLoadFloat4x4(&m.ViewMatrix);
		ViewMat = XMMatrixInverse(NULL, ViewMat);
		XMMATRIX TransMat = XMMatrixTranslationFromVector(XMLoadFloat3(&move) * (float)xTime.Delta());
		ViewMat = XMMatrixMultiply(TransMat, ViewMat);
		ViewMat = XMMatrixInverse(NULL, ViewMat);
		XMStoreFloat4x4(&m.ViewMatrix, ViewMat);//*/
	//Also works just condensed into one line
/*		XMStoreFloat4x4(&Scene.ViewMatrix, 
			XMMatrixInverse(NULL, XMMatrixMultiply(
			XMMatrixTranslationFromVector(XMLoadFloat4x4(&move) * (float)xTime.Delta()), 
			XMMatrixInverse(NULL, XMLoadFloat4x4(&Scene.ViewMatrix)))));//*/
	}
}
