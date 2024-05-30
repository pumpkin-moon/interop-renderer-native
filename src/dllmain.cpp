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
Image NewImage(ImmediateAPI* handle, byte* data, int length)
{
    return handle->NewImage(data, length);
}

DLL_API
Image NewImageRaw(ImmediateAPI* handle, byte* data, int width, int height, int channels)
{
    return handle->NewImageRaw(data, width, height, channels);
}

DLL_API
Font NewFont(ImmediateAPI* handle, byte* data, int length, float size)
{
    return handle->NewFont(data, length, size);
}

DLL_API
Shader NewShader(ImmediateAPI* handle, char* name, char* vertexName, char* pixelName, byte* data, int length) 
{
    return handle->NewShader(name, vertexName, pixelName, data, length);
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