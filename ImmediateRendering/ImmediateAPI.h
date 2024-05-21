#pragma once

#include <windows.h>
#include <windowsx.h>

#include <d3d11.h>

#pragma comment (lib, "d3d11.lib")

#include "shapes.h"
#include "Mat3x2.h"

struct ImmediateAPI
{
	IDXGISwapChain* swapchain;             
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	ID3D11RenderTargetView* backbuffer;

	static ImmediateAPI* init(HWND hWnd);
	void begin_frame(unsigned int color);
	void end_frame();
	void release();
	void resize(int width, int height);

	ID3D11ShaderResourceView* create_image(void* data, int length, int width, int height);

	void set_matrix(Mat3x2 matrix);

	void draw_circle(Circle circle);
	void draw_line(Line line);
	void draw_image(Image image);
};