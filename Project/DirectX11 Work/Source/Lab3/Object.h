#ifndef OBJECT_H
#define OBJECT_H

#include "../Include.h"

class Object{
	XMFLOAT4X4		WorldMatrix;
	ID3D11Buffer	*iConstBuffer;
	ID3D11Buffer	*iVertexBuffer;
	ID3D11Buffer	*iIndexBuffer;
	unsigned short	VertexCount;
	unsigned short	IndexCount;

	//Rate of Movement
	XMFLOAT3		Velocity;
	//Rate of Rotation
	XMFLOAT3		Rotation;

public:
	Object();
	~Object();

	HRESULT MapConstBuffer(ID3D11DeviceContext *iContext);

	void Update(XTime xTime);

//Buffer Functions
	void	Release				()	;
	void	ReleaseConstBuffer	()	{ iConstBuffer->Release();	}
	void	ReleaseVertexBuffer	()	{ iVertexBuffer->Release();	}
	void	ReleaseIndexBuffer	()	{ iIndexBuffer->Release();	}

//Accessors
	XMFLOAT3	GetVelocityXMF3		()	{ return Velocity;						}
	XMVECTOR	GetVelocityXMV		()	{ return XMLoadFloat3(&Velocity);		}
	XMFLOAT3	GetRotationRateXMF3	()	{ return Rotation;						}
	XMVECTOR	GetRotationRateXMV	()	{ return XMLoadFloat3(&Rotation);		}
	XMFLOAT4X4	GetWorldMatrixXMF4X4()	{ return WorldMatrix;					}
	XMMATRIX	GetWorldMatrixXMM	()	{ return XMLoadFloat4x4(&WorldMatrix);	}

	unsigned short	GetVertexCount	()	{ return VertexCount;	}
	unsigned short	GetIndexCount	()	{ return IndexCount;	}

	ID3D11Buffer*	GetConstBuffer	()	{ return iConstBuffer;	}
	ID3D11Buffer*	GetVertexBuffer	()	{ return iVertexBuffer;	}
	ID3D11Buffer*	GetIndexBuffer	()	{ return iIndexBuffer;	}
	
	ID3D11Buffer** GetConstBufferAddress	()	{ return &iConstBuffer;		}
	ID3D11Buffer** GetVertexBufferAddress	()	{ return &iVertexBuffer;	}
	ID3D11Buffer** GetIndexBufferAddress	()	{ return &iIndexBuffer;		}
	ID3D11Buffer* const* GetConstBufferAdd	()	{ return &iConstBuffer;		}
	ID3D11Buffer* const* GetVertexBufferAdd	()	{ return &iVertexBuffer;	}
	ID3D11Buffer* const* GetIndexBufferAdd	()	{ return &iIndexBuffer;		}

//Mutators
	void	SetConstBuffer	(ID3D11Buffer *pBuffer)	{ iConstBuffer  = pBuffer;	}
	void	SetVertexBuffer	(ID3D11Buffer *pBuffer)	{ iVertexBuffer = pBuffer;	}
	void	SetIndexBuffer	(ID3D11Buffer *pBuffer)	{ iIndexBuffer  = pBuffer;	}

	void	SetVelocityXMF3		(XMFLOAT3 vVel)		{ Velocity = vVel;					}
	void	SetVelocityXMV		(XMVECTOR vVel)		{ XMStoreFloat3(&Velocity, vVel);	}
	void	SetRotationRateXMF3	(XMFLOAT3 vVel)		{ Rotation = vVel;					}
	void	SetRotationRateXMV	(XMVECTOR vVel)		{ XMStoreFloat3(&Rotation, vVel);	}
	void	SetWorldMatrixXMF4X4(XMFLOAT4X4 mM)		{ WorldMatrix = mM;					}
	void	SetWorldMatrixXMM	(XMMATRIX  &mM)		{ XMStoreFloat4x4(&WorldMatrix, mM);}
	void	SetPosition			(XMFLOAT3 vPos)		;

	void	SetVertexCount	(unsigned short usCount) { VertexCount = usCount;	}
	void	SetIndexCount	(unsigned short usCount) { IndexCount = usCount;	}
};

#endif