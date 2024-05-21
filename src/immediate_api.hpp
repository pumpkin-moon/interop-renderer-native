#pragma once
#include "windows.h"
#include "d3d11.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

struct ImmediateAPI {
private:
	ImmediateAPI* self;
	HWND window;

	IDXGISwapChain* swapchain;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11RenderTargetView* backbuffer;

public:
	static ImmediateAPI* Initialize(HWND hWnd);
	void Release();

private:	
	void InitD3D();
	void InitImGui();
	void ResizeDevice(int width, int height);
};