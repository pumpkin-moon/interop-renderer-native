#pragma once

#include <windows.h>
#include <windowsx.h>

#include <d3d11.h>

#pragma comment (lib, "d3d11.lib")

#include "shapes.h"
#include "Mat3x2.h"

struct ImmediateAPI
{
private:
	IDXGISwapChain* swapchain;             
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11RenderTargetView* backbuffer;

public:
	static ImmediateAPI* Initialize(HWND hWnd);
	void BeginFrame(unsigned int color);
	void EndFrame();
	void Release();
	void Resize(int width, int height);

	ID3D11ShaderResourceView* CreateImage(void* data, int length, int width, int height);

	void DrawList(byte* data, int length);

private:
	void InitD3D(HWND hWnd);
	void ResizeInternal(int width, int height);
};