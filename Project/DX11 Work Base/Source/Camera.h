#ifndef CAMERA_H
#define CAMERA_H

#include "../../DirectX11 Work/Source/Include.h"

#pragma region Defines
#define CAMERA_MOVE_X 30.0f
#define CAMERA_MOVE_Y 30.0f
#define CAMERA_MOVE_Z 30.0f
#define CAMERA_ROT_X 30.0f
#define CAMERA_ROT_Y 30.0f
#define CAMERA_ROT_Z 30.0f
#pragma endregion

struct MatPack{
	XMFLOAT4X4 WorldMatrix;
	XMFLOAT4X4 ViewMatrix;
	XMFLOAT4X4 ProjMatrix;
};

class Camera{
	void SnapMove(XTime xTime);
//	void Rotate(XTime xTime);

	MatPack m;
	ID3D11Buffer	*iConstBuffer;
public:
	Camera();
	~Camera();
	
	MatPack*			 GetMatrixPack		()						{ return &m;				}
	ID3D11Buffer*		 GetConstBuffer		()						{ return iConstBuffer;		}
	ID3D11Buffer* const* GetConstBufferAdd	()						{ return &iConstBuffer;		}
	void				 SetConstBuffer		(ID3D11Buffer *pBuffer)	{ iConstBuffer = pBuffer;	}
	void				 ReleaseConstBuffer	()						{ iConstBuffer->Release();	}

	void Update(XTime xTime);
};

#endif