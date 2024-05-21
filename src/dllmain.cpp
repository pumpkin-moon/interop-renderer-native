#include "immediate_api.hpp"

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
