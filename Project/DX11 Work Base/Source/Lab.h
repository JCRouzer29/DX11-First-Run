#ifndef LAB_H
#define LAB_H

#include "../../DirectX11 Work/Source/Include.h"

struct Lab{
public:
	Lab() {}
	~Lab(){}

	bool Setup(HWND window)	{ return true; }
	bool Run(XTime xTime)	{ return true; }
	bool ShutDown()			{ return true; }
};

#endif