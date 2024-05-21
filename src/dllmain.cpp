#include "immediate_api.hpp"
#include "draw_api.hpp"

#define DLL_API extern "C" __declspec(dllexport)

DLL_API
ImmediateAPI* Initialize(HWND hWnd) 
{
    return ImmediateAPI::Initialize(hWnd);
}

DLL_API
void Release(ImmediateAPI* handle)
{
    handle->Release();
}

DLL_API
void Resize(ImmediateAPI* handle, int width, int height)
{
    handle->Resize(width, height);
}

DLL_API
DrawAPI CreateBuffer()
{
    return DrawAPI::Create();
}

DLL_API
Image CreateImage(ImmediateAPI* handle, void* data, int length)
{
    return handle->CreateImage(data, length);
}

DLL_API
void BeginFrame(ImmediateAPI* handle, Color color)
{
    handle->BeginFrame(color);
}

DLL_API
void EndFrame(ImmediateAPI* handle)
{
    handle->EndFrame();
}