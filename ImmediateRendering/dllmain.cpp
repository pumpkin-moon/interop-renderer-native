#include "ImmediateAPI.h"

#define EXTERN extern "C" __declspec(dllexport)

EXTERN
ImmediateAPI* Init(HWND hWnd)
{
    return ImmediateAPI::init(hWnd);
}

EXTERN
void Render(ImmediateAPI* handle)
{
    handle->render();
}

EXTERN
void RenderCircles(ImmediateAPI* handle, Circle* circles, int count)
{
    handle->render_circles(circles, count);
}

EXTERN
void Release(ImmediateAPI* handle) 
{
    handle->release();
}

EXTERN
void Resize(ImmediateAPI* handle, int width, int height)
{
    handle->resize(width, height);
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

