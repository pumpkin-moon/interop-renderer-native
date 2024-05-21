#include "immediate_api.hpp"
#include "iostream"

ImmediateAPI* ImmediateAPI::Initialize(HWND hWnd) 
{
    auto api = (ImmediateAPI*)malloc(sizeof(ImmediateAPI));

    if (api == NULL) {
        return NULL;
    }

    api->self = api;
    api->window = hWnd;

    api->InitD3D();
    api->InitImGui();

    return api;
}

void ImmediateAPI::Release() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    swapchain->Release();
    backbuffer->Release();
    device->Release();
    context->Release();

    free(self);
}

void ImmediateAPI::InitD3D()
{
    DXGI_SWAP_CHAIN_DESC scd;

    // clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    // fill the swap chain description struct
    scd.BufferCount = 1;                                    // one back buffer
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
    scd.OutputWindow = window;                              // the window to be used
    scd.SampleDesc.Count = 4;                               // how many multisamples
    scd.Windowed = TRUE;                                    // windowed/full-screen mode

    // create a device, device context and swap chain using the information in the scd struct
    D3D11CreateDeviceAndSwapChain(NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        NULL,
        NULL,
        NULL,
        D3D11_SDK_VERSION,
        &scd,
        &swapchain,
        &device,
        NULL,
        &context);

    RECT rect;
    GetWindowRect(window, &rect);
    ResizeDevice(rect.right - rect.left, rect.bottom - rect.top);
}

void ImmediateAPI::InitImGui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(device, context);
}

void ImmediateAPI::ResizeDevice(int width, int height)
{
    // get the address of the back buffer
    ID3D11Texture2D* pBackBuffer;
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (pBackBuffer == NULL)
    {
        std::cout << "'pBackBuffer' is null!" << std::endl;
        return;
    }

    // use the back buffer address to create the render target
    device->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
    pBackBuffer->Release();

    // set the render target as the back buffer
    context->OMSetRenderTargets(1, &backbuffer, NULL);

    // Set the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = float(width);
    viewport.Height = float(height);

    context->RSSetViewports(1, &viewport);
}
