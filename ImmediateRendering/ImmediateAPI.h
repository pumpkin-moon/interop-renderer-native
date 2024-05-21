#pragma once
// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include "circle.h"

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")

struct ImmediateAPI
{
	LPDIRECT3D9 d3d;    
	LPDIRECT3DDEVICE9 device;    

	static ImmediateAPI* init(HWND hWnd);
	void render();
	void render_circles(Circle* circles, int count);
	void release();
	void resize(int width, int height);
};