#include "ImmediateAPI.h"
#include "Mat3x2.h"

#define EXTERN extern "C" __declspec(dllexport)

EXTERN
ImmediateAPI* Init(HWND hWnd)
{
    return ImmediateAPI::init(hWnd);
}

EXTERN
void BeginFrame(ImmediateAPI* handle, unsigned int color)
{
    handle->begin_frame(color);
}

EXTERN
void EndFrame(ImmediateAPI* handle)
{
    handle->end_frame();
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

EXTERN 
void SetMatrix(ImmediateAPI* handle, Mat3x2 matrix)
{
    handle->set_matrix(matrix);
}

EXTERN
void* CreateImage(ImmediateAPI* handle, void* data, int length, int width, int height)
{
    return handle->create_image(data, length, width, height);
}

EXTERN
void DrawCircle(ImmediateAPI* handle, Circle circle)
{
    handle->draw_circle(circle);
}

EXTERN
void DrawLine(ImmediateAPI* handle, Line line)
{
    handle->draw_line(line);
}

EXTERN 
void DrawImage(ImmediateAPI* handle, Image image)
{
    handle->draw_image(image);
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

