#include "ImmediateAPI.h"
#include "Mat3x2.h"

#define EXTERN extern "C" __declspec(dllexport)

EXTERN
ImmediateAPI* Init(HWND hWnd)
{
    return ImmediateAPI::Initialize(hWnd);
}

EXTERN
void BeginFrame(ImmediateAPI* handle, unsigned int color)
{
    handle->BeginFrame(color);
}

EXTERN
void EndFrame(ImmediateAPI* handle)
{
    handle->EndFrame();
}

EXTERN
void Release(ImmediateAPI* handle) 
{
    handle->Release();
}

EXTERN
void Resize(ImmediateAPI* handle, int width, int height)
{
    handle->Resize(width, height);
}

EXTERN
void* CreateImage(ImmediateAPI* handle, void* data, int length, int width, int height)
{
    return handle->CreateImage(data, length, width, height);
}

EXTERN
void DrawList(ImmediateAPI* handle, byte* list, int length)
{
    handle->DrawList(list, length);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

