#pragma once
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "d3d11.h"
#pragma comment (lib, "d3d11.lib")

#include "drawing.hpp"
#include "draw_api.hpp"

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
	static ImDrawList* GetDrawList();

	void Release();

	Image CreateImage(void* data, int length);

	void BeginFrame(Color color);
	void EndFrame();

private:	
	void InitD3D();
	void InitImGui();
	void ResizeDevice(int width, int height);
};